#include "font.h"
#include <iostream>
#include "gui.h"

namespace guistorm {

#ifdef GUISTORM_NO_UTF
  GLfloat font::glyph::get_kerning(char charcode_last) const {
#else
  GLfloat font::glyph::get_kerning(char32_t charcode_last) const {
#endif // GUISTORM_NO_UTF
  /// Return the kerning for this glyph when preceded by the specified previous glyph
  #ifdef GUISTORM_NO_UTF
    if(charcode_last == '\0') {
  #else
    if(charcode_last == U'\0') {
  #endif // GUISTORM_NO_UTF
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
  #ifdef GUISTORM_NO_UTF
    char charcode_last = ' ';
  #else
    char32_t charcode_last = U' ';
  #endif // GUISTORM_NO_UTF
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

font::font(gui *new_parent_gui,
           std::string const &new_name,
           unsigned char const *new_memory_offset,
           size_t new_memory_size,
           float new_font_size,
           #ifdef GUISTORM_NO_UTF
             std::string const &charcodes_to_load,
           #else
             std::u32string const &charcodes_to_load,
           #endif // GUISTORM_NO_UTF
           bool new_suppress_horizontal_hint)
  : parent_gui(new_parent_gui),
    name(new_name),
    memory_offset(new_memory_offset),
    memory_size(new_memory_size),
    font_size(new_font_size),
    charcodes(charcodes_to_load),
    suppress_horizontal_hint(new_suppress_horizontal_hint) {
  /// Default specific constructor
  #ifndef NDEBUG
    if(!parent_gui) {
      std::cout << "GUIStorm: Font: ERROR: attempting to create a font with no valid parent!" << std::endl;
    }
  #endif
  if(charcodes.empty()) {                                                       // no custom glyphs specified, so load a sensible default selection
    // note: space needs to be the first character, as it's used for reference elsewhere
    #ifdef GUISTORM_NO_UTF
      //charcodes = " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,?!:/"; // common glyphs
      charcodes = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\t\n\r"; // all lower ascii plus whitespace
    #else
      //charcodes = U" ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,?!:/"; // common glyphs
      charcodes = U" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\t\n\r"; // all lower ascii plus whitespace
    #endif // GUISTORM_NO_UTF
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
    if(glfwGetCurrentContext() == NULL) {                                       // make sure we're in a valid opengl context before we try to refresh
      std::cout << "GUIStorm: WARNING: Attempting to load fonts with no current GL context, ignoring." << std::endl;
      return false;
    }
    if(!font_atlas) {                                                           // make sure we have a font atlas ready to populate
      std::cout << "GUIStorm: WARNING: Attempting to load fonts with no font atlas loaded, creating one now..." << std::endl;
      return false;
    }
  #endif
  unload();
  FT_Library library;
  FT_Init_FreeType(&library);                                                   // initialise library
  FT_Face face;
  FT_New_Memory_Face(library, memory_offset, memory_size, 0, &face);            // load face
  FT_Select_Charmap(face, FT_ENCODING_UNICODE);                                 // select charmap
  if(suppress_horizontal_hint) {                                                // http://www.antigrain.com/research/font_rasterization/ http://jcgt.org/published/0002/01/04/
    FT_Set_Char_Size(face,
                     0,
                     static_cast<FT_F26Dot6>(font_size * hres),
                     static_cast<FT_UInt>(parent_gui->get_dpi() * horizontal_hint_suppression),
                     static_cast<FT_UInt>(parent_gui->get_dpi()));              // stretched for hint suppression
    FT_Matrix matrix = {static_cast<int>((1.0f / horizontal_hint_suppression) * 0x10000l), 0, 0, 0x10000l}; // magic number for 16:16 fixed point
    FT_Set_Transform(face, &matrix, nullptr);                                   // set transform matrix
  } else {
    FT_Set_Char_Size(face,
                     0,
                     static_cast<FT_F26Dot6>(font_size * hres),
                     static_cast<FT_UInt>(parent_gui->get_dpi()),
                     static_cast<FT_UInt>(parent_gui->get_dpi()));              // set char size
    FT_Set_Transform(face, nullptr, nullptr);                                   // set transform matrix - identity
  }

  // cache the overall metrics
  FT_Size_Metrics const &metrics = face->size->metrics;
  metrics_ascender  = static_cast<GLfloat>(metrics.ascender  >> 6);
  metrics_descender = static_cast<GLfloat>(metrics.descender >> 6);
  metrics_height    = static_cast<GLfloat>(metrics.height    >> 6);
  metrics_linegap   = metrics_height - metrics_ascender + metrics_descender;
  #ifdef DEBUG_GUISTORM
    std::cout << "GUIStorm: Loading font " << name << " (" << memory_size / 1024 << "KB) size " << font_size << " (height " << metrics_height << ", " << glyphs.size() << " glyphs)" << std::endl;
  #endif // DEBUG_GUISTORM

  // load each glyph
  if(!load_glyphs(font_atlas, face, charcodes)) {
    std::cout << "GUIStorm: WARNING: Failed to load all glyphs." << std::endl;
    return false;
  }

  // calculate kerning for each glyph pair
  update_kerning(face);

  FT_Done_Face(face);
  FT_Done_FreeType(library);
  return true;
}

#ifdef GUISTORM_NO_UTF
  bool font::load_glyphs(freetypeglxx::TextureAtlas *font_atlas, FT_Face const &face, std::string const &codes_to_load) {
#else
  bool font::load_glyphs(freetypeglxx::TextureAtlas *font_atlas, FT_Face const &face, std::u32string const &codes_to_load) {
#endif // GUISTORM_NO_UTF
  /// Load all glyphs specified in a string
  for(auto const &thischar : codes_to_load) {
    if(!load_glyph(font_atlas, face, thischar)) {
      std::cout << "GUIStorm: WARNING: Failed to load glyph \"" << thischar << "\" (ascii " << static_cast<unsigned int>(thischar) << ")" << std::endl;
      return false;
    }
  }
  return true;
}
#ifdef GUISTORM_NO_UTF
  bool font::load_glyph(freetypeglxx::TextureAtlas *font_atlas, FT_Face const &face, char thischar) {
#else
  bool font::load_glyph(freetypeglxx::TextureAtlas *font_atlas, FT_Face const &face, char32_t thischar) {
#endif // GUISTORM_NO_UTF
  /// Load a glyph specified by one UTF32 codepoint
  FT_UInt glyph_index = FT_Get_Char_Index(face, thischar);
  FT_Int32 flags = 0;
  //flags |= FT_LOAD_NO_BITMAP;                                                 // freetype-gl default when using outlines
  flags |= FT_LOAD_RENDER;                                                      // freetype-gl default when using normal rendering
  if(force_autohint) {
    flags |= FT_LOAD_FORCE_AUTOHINT;                                            // freetype-gl default when hinting enabled
  } else {
    if(suppress_autohunt) {
      flags |= FT_LOAD_NO_AUTOHINT;                                             // freetype-gl default when hinting disabled
    }
    if(suppress_hinting) {
      flags |= FT_LOAD_NO_HINTING;                                              // freetype-gl default when hinting disabled
    }
  }
  FT_Load_Glyph(face, glyph_index, flags);

  FT_Bitmap ft_bitmap = face->glyph->bitmap;
  // We want each glyph to be separated by at least one blank pixel (eg. shader in demo-subpixel.c)
  vec2<size_t> bitmap_size(ft_bitmap.width / font_atlas->depth() + 1, ft_bitmap.rows + 1);
  freetypeglxx::ivec4 region = font_atlas->GetRegion(bitmap_size.x, bitmap_size.y);
  if(region.x < 0) {
    std::cout << "GUIStorm: WARNING: font load: no room in atlas for font " << name << " at size " << font_size << std::endl;
    return false;                                                               // we've missed a glyph so drop out early to retry
  }
  bitmap_size -= 1;
  font_atlas->SetRegion(region.x, region.y, bitmap_size.x, bitmap_size.y, ft_bitmap.buffer, ft_bitmap.pitch);

  std::shared_ptr<font::glyph> tempglyph(new glyph);
  tempglyph->charcode    = thischar;
  tempglyph->offset.x    = static_cast<GLfloat>(face->glyph->bitmap_left);
  tempglyph->offset.y    = static_cast<GLfloat>(face->glyph->bitmap_top) - static_cast<GLfloat>(bitmap_size.y);
  tempglyph->size.x      = static_cast<GLfloat>(bitmap_size.x);
  tempglyph->size.y      = static_cast<GLfloat>(bitmap_size.y);
  tempglyph->texcoord0.x = static_cast<GLfloat>( region.x                 ) / static_cast<GLfloat>(font_atlas->width());
  tempglyph->texcoord0.y = static_cast<GLfloat>((region.y + bitmap_size.y)) / static_cast<GLfloat>(font_atlas->height()); // y is flipped for texture coords
  tempglyph->texcoord1.x = static_cast<GLfloat>((region.x + bitmap_size.x)) / static_cast<GLfloat>(font_atlas->width());
  tempglyph->texcoord1.y = static_cast<GLfloat>( region.y                 ) / static_cast<GLfloat>(font_atlas->height()); // y is flipped for texture coords
  FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER | FT_LOAD_NO_HINTING);        // discard hinting to get advance
  tempglyph->advance.x   = static_cast<GLfloat>(face->glyph->advance.x) / hres;
  tempglyph->advance.y   = static_cast<GLfloat>(face->glyph->advance.y) / hres;

  #ifdef GUISTORM_NO_UTF
    if(thischar == ' ') {                                                       // if we're drawing whitespace, skip adding the quad - every little helps
  #else
    if(thischar == U' ') {                                                      // if we're drawing whitespace, skip adding the quad - every little helps
  #endif // GUISTORM_NO_UTF
    tempglyph->is_blank = true;
  #ifdef GUISTORM_NO_UTF
    } else if(thischar == '\t') {                                               // tab
  #else
    } else if(thischar == U'\t') {                                              // tab
  #endif // GUISTORM_NO_UTF
    tempglyph->is_blank = true;
  #ifdef GUISTORM_NO_UTF
    tempglyph->advance.x = 4.0f * getglyph(' ')->advance.x;                     // use four spaces for a tab - yes lame
    } else if(thischar == '\n' || thischar == '\r') {                           // newline or carriage return
  #else
    tempglyph->advance.x = 4.0f * getglyph(U' ')->advance.x;                    // use four spaces for a tab - yes lame
    } else if(thischar == U'\n' || thischar == U'\r') {                         // newline or carriage return
  #endif // GUISTORM_NO_UTF
    tempglyph->is_blank = true;
    tempglyph->linebreak = true;
    //tempglyph->advance.x = 0.0f;                                              // newlines do not advance the cursor
  }

  {
    std::lock_guard<std::mutex> lock(glyph_map_mutex);
    glyphs.emplace(thischar, tempglyph);
  }
  return true;
}

void font::unload() {
  /// Unload this font from memory
  /// Note: it is not usually necessary to call this explicitly, as load() will unload first, and destruction will clean up properly
  std::lock_guard<std::mutex> lock(glyph_map_mutex);
  glyphs.clear();
}

void font::update_kerning(FT_Face const &face) {
  /// Update the kerning between all glyphs
  for(auto const &this_glyph : glyphs) {
    update_kerning(face, *this_glyph.second);
  }
}
void font::update_kerning(FT_Face const &face, glyph &this_glyph) {
  /// Update the kerning between this glyph and all others preceding
  this_glyph.kerning.clear();
  for(auto const &last_glyph : glyphs) {
    update_kerning(face, this_glyph, *last_glyph.second);
  }
}
void font::update_kerning(FT_Face const &face, glyph &this_glyph, glyph const &last_glyph) {
  /// Update the kerning between two glyphs
  FT_UInt glyph_index = FT_Get_Char_Index(face, this_glyph.charcode);
  FT_UInt prev_index  = FT_Get_Char_Index(face, last_glyph.charcode);
  FT_Vector kerning;
  FT_Get_Kerning(face, prev_index, glyph_index, FT_KERNING_UNFITTED, &kerning);
  this_glyph.kerning.emplace(last_glyph.charcode, static_cast<GLfloat>(kerning.x) / (hres * hres));
}

#ifdef GUISTORM_NO_UTF
  std::shared_ptr<font::glyph> const font::getglyph(char charcode) {
#else
  std::shared_ptr<font::glyph> const font::getglyph(char32_t charcode) {
#endif // GUISTORM_NO_UTF
  /// Reimplemented form of TextureFont::GetGlyph which is a wrapper for texture_font_load_glyphs
  std::shared_ptr<font::glyph> tempglyph;
  try {
    tempglyph = glyphs.at(charcode);
  } catch(std::out_of_range const &e) {
    #ifdef GUISTORM_LOAD_MISSING_GLYPHS
      std::cout << "GUIStorm: loading glyph for character \"" << charcode << "\" (ascii " << static_cast<unsigned int>(charcode) << ")" << std::endl;
      ///unload();
      ///__sync_synchronize();                                                  // memory barrier
      charcodes += charcode;
      parent_gui->load_fonts();                                                 // request a full font reload - expensive!
      tempglyph = glyphs[charcode];
    #else
      std::cout << "GUIStorm: WARNING: could not fetch glyph for character \"" << charcode << "\" (ascii " << static_cast<unsigned int>(charcode) << ")" << std::endl;
    #endif // GUISTORM_LOAD_MISSING_GLYPHS
  }
  return tempglyph;
}

}
