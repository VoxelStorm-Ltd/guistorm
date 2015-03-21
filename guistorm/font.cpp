#include "font.h"
#include <iostream>
#include <ft2build.h>
#include <freetype/freetype.h>
#include "gui.h"

namespace guistorm {


GLfloat font::glyph::get_kerning(char charcode_last) const {
  /// Return the kerning for this glyph when preceded by the specified previous glyph
  if(charcode_last == '\0') {
    return 0.0f;
  }
  GLfloat result = 0.0f;
  try {
    #ifdef DEBUG_GUISTORM
      //std::cout << "GUIStorm: DEBUG: kerning from \"" << charcode_last << "\" (ascii " << static_cast<int>(charcode_last) << ")" << " to \"" << charcode << "\" (ascii " << static_cast<int>(charcode) << ")" << std::endl;
    #endif
    result = kerning.at(charcode_last);
  } catch(std::out_of_range const &e) {
    result = 0.0f;
  }
  return result;
}

GLfloat font::word::length() const {
  /// Return the horizontal length of this word
  GLfloat length = 0.0f;
  char charcode_last = ' ';
  for(auto const &thisglyph : glyphs) {
    length += thisglyph->advance.x + thisglyph->get_kerning(charcode_last);
    charcode_last = thisglyph->charcode;
  }
  return length;
}

GLfloat font::line::length() const {
  /// Return the horizontal length of this line
  GLfloat length = 0.0f;
  for(auto const &thisword : words) {
    length += thisword.length();
  }
  return length;
}

font::font(gui *parent_gui,
           std::string const &name,
           const unsigned char* memory_offset,
           size_t memory_size,
           float font_size,
           std::string const &charcodes_to_load,
           bool suppress_horizontal_hint)
  : parent_gui(parent_gui),
    name(name),
    memory_offset(memory_offset),
    memory_size(memory_size),
    font_size(font_size),
    charcodes(charcodes_to_load),
    suppress_horizontal_hint(suppress_horizontal_hint) {
  /// Default specific constructor
  #ifndef NDEBUG
    if(!parent_gui) {
      std::cout << "GUIStorm: Font: ERROR: attempting to create a font with no valid parent!" << std::endl;
    }
  #endif
  if(charcodes.empty()) {        // no custom glyphs specified, so load a sensible default selection
    // note: space needs to be the first character, as it's used for reference elsewhere
    //charcodes = " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,?!:/";    // common glyphs
    charcodes = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\t\n\r";  // all lower ascii plus whitespace
  }
}

font::~font() {
  /// Default destructor
  unload();
}

bool font::load_if_needed(freetypeglxx::TextureAtlas *font_atlas) {
  /// Wrapper to check if this font is unloaded, and if so load it
  if(glyphs.empty()) {
    return load(font_atlas);
  } else {
    return true;
  }
}

bool font::load(freetypeglxx::TextureAtlas *font_atlas) {
  /// Attempt to load this font into the specified font atlas
  /// Reimplemented form of TextureFont::LoadGlyphs which is a wrapper for texture_font_load_glyphs
  #ifndef NDEBUG
    if(glfwGetCurrentContext() == NULL) {     // make sure we're in a valid opengl context before we try to refresh
      std::cout << "GUIStorm: WARNING: Attempting to load fonts with no current GL context, ignoring." << std::endl;
      return false;
    }
    if(!font_atlas) {                         // make sure we have a font atlas ready to populate
      std::cout << "GUIStorm: WARNING: Attempting to load fonts with no font atlas loaded, creating one now..." << std::endl;
      return false;
    }
  #endif
  unload();
  GLfloat constexpr hres = 64;                                                            // from #define HRES 64 - Freetype uses 1/64th of a point scale
  FT_Library library;
  FT_Init_FreeType(&library);                                                             // initialise library
  FT_Face face;
  FT_New_Memory_Face(library, memory_offset, memory_size, 0, &face);                      // load face
  FT_Select_Charmap(face, FT_ENCODING_UNICODE);                                           // select charmap
  if(suppress_horizontal_hint) {                                                          // http://www.antigrain.com/research/font_rasterization/ http://jcgt.org/published/0002/01/04/
    FT_Set_Char_Size(face, 0, font_size * hres, parent_gui->get_dpi() * horizontal_hint_suppression, parent_gui->get_dpi());    // stretched for hint suppression
    FT_Matrix matrix = {static_cast<int>((1.0f / horizontal_hint_suppression) * 0x10000l), 0, 0, 0x10000l};                     // magic number for 16:16 fixed point
    FT_Set_Transform(face, &matrix, nullptr);                                             // set transform matrix
  } else {
    FT_Set_Char_Size(face, 0, font_size * hres, parent_gui->get_dpi(), parent_gui->get_dpi());                                  // set char size
    FT_Set_Transform(face, nullptr, nullptr);                                             // set transform matrix - identity
  }

  // cache the overall metrics
  FT_Size_Metrics metrics = face->size->metrics;
  metrics_ascender  = metrics.ascender  >> 6;
  metrics_descender = metrics.descender >> 6;
  metrics_height    = metrics.height    >> 6;
  metrics_linegap   = metrics_height - metrics_ascender + metrics_descender;
  #ifdef DEBUG_GUISTORM
    std::cout << "GUIStorm: Loading font " << name << " (" << memory_size / 1024 << "KB) size " << font_size << " (height " << metrics_height << ", " << glyphs.size() << " glyphs)" << std::endl;
  #endif // DEBUG_GUISTORM

  // load each glyph
  for(size_t i = 0; i != charcodes.size(); ++i) {
    FT_UInt glyph_index = FT_Get_Char_Index(face, charcodes[i]);
    FT_Int32 flags = 0;
    //flags |= FT_LOAD_NO_BITMAP;           // freetype-gl default when using outlines
    flags |= FT_LOAD_RENDER;                // freetype-gl default when using normal rendering
    if(force_autohint) {
      flags |= FT_LOAD_FORCE_AUTOHINT;      // freetype-gl default when hinting enabled
    } else {
      if(suppress_autohunt) {
        flags |= FT_LOAD_NO_AUTOHINT;       // freetype-gl default when hinting disabled
      }
      if(suppress_hinting) {
        flags |= FT_LOAD_NO_HINTING;        // freetype-gl default when hinting disabled
      }
    }
    FT_Load_Glyph(face, glyph_index, flags);

    FT_Bitmap ft_bitmap = face->glyph->bitmap;
    // We want each glyph to be separated by at least one blank pixel (eg. shader in demo-subpixel.c)
    Vector2<size_t> bitmap_size(ft_bitmap.width / font_atlas->depth() + 1, ft_bitmap.rows + 1);
    freetypeglxx::ivec4 region = font_atlas->GetRegion(bitmap_size.x, bitmap_size.y);
    if(region.x < 0) {
      std::cout << "GUIStorm: WARNING: font load: no room in atlas for font " << name << " at size " << font_size << std::endl;
      return false;                       // we've missed a glyph so drop out early to retry
    }
    bitmap_size -= 1;
    font_atlas->SetRegion(region.x, region.y, bitmap_size.x, bitmap_size.y, ft_bitmap.buffer, ft_bitmap.pitch);

    glyph *tempglyph = new glyph;
    tempglyph->charcode    = charcodes[i];
    tempglyph->offset.x    = face->glyph->bitmap_left;
    tempglyph->offset.y    = static_cast<GLfloat>(face->glyph->bitmap_top) - bitmap_size.y;
    tempglyph->size.x      = bitmap_size.x;
    tempglyph->size.y      = bitmap_size.y;
    tempglyph->texcoord0.x =  region.x                  / static_cast<GLfloat>(font_atlas->width());
    tempglyph->texcoord0.y = (region.y + bitmap_size.y) / static_cast<GLfloat>(font_atlas->height()); // y is flipped for texture coords
    tempglyph->texcoord1.x = (region.x + bitmap_size.x) / static_cast<GLfloat>(font_atlas->width());
    tempglyph->texcoord1.y =  region.y                  / static_cast<GLfloat>(font_atlas->height()); // y is flipped for texture coords
    FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER | FT_LOAD_NO_HINTING);                          // discard hinting to get advance
    tempglyph->advance.x   = face->glyph->advance.x / hres;
    tempglyph->advance.y   = face->glyph->advance.y / hres;

    if(charcodes[i] == L' ') {                                    // if we're drawing whitespace, skip adding the quad - every little helps
      tempglyph->is_blank = true;
    } else if(charcodes[i] == L'\t') {                            // tab
      tempglyph->is_blank = true;
      tempglyph->advance.x = 4.0f * getglyph(' ')->advance.x;     // use four spaces for a tab - yes lame
    } else if(charcodes[i] == L'\n' || charcodes[i] == L'\r') {   // newline or carriage return
      tempglyph->is_blank = true;
      tempglyph->linebreak = true;
      ///tempglyph->advance.x = 0.0f;                                // newlines do not advance the cursor
    }

    glyphs.emplace(charcodes[i], tempglyph);
  }

  // calculate kerning for each glyph pair
  for(auto const &thisglyph : glyphs) {
    thisglyph.second->kerning.clear();
    FT_UInt glyph_index = FT_Get_Char_Index(face, thisglyph.second->charcode);
    for(auto const &lastglyph : glyphs) {
      FT_UInt prev_index = FT_Get_Char_Index(face, lastglyph.second->charcode);
      FT_Vector kerning;
      FT_Get_Kerning(face, prev_index, glyph_index, FT_KERNING_UNFITTED, &kerning);
      thisglyph.second->kerning.emplace(lastglyph.second->charcode, kerning.x / (hres * hres));
    }
  }

  FT_Done_Face(face);
  FT_Done_FreeType(library);
  return true;
}

void font::unload() {
  /// Unload this font from memory
  /// Note: it is not usually necessary to call this explicitly, as load() will unload first, and destruction will clean up properly
  for(auto const &it : glyphs) {
    delete it.second;
  }
  glyphs.clear();
}

font::glyph const *font::getglyph(char charcode) {
  /// Reimplemented form of TextureFont::GetGlyph which is a wrapper for texture_font_load_glyphs
  font::glyph *tempglyph = nullptr;
  try {
    tempglyph = glyphs.at(charcode);
  } catch(std::out_of_range const &e) {
    std::cout << "GUIStorm: WARNING: could not fetch glyph for character \"" << charcode << "\" (ascii " << static_cast<int>(charcode) << ")" << std::endl;
  }
  return tempglyph;
}

}
