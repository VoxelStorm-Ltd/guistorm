#ifndef GUISTORM_FONT_H_INCLUDED
#define GUISTORM_FONT_H_INCLUDED

#ifndef GUISTORM_NO_TEXT
  #include <string>
  #include <vector>
  #include <unordered_map>
  #include <mutex>
  #include <memory>
  #include <ft2build.h>
  #include FT_FREETYPE_H
  #include <freetype-gl++/freetype-gl++.hpp>
  #include "types.h"
#endif // GUISTORM_NO_TEXT

namespace guistorm {

class gui;

#ifdef GUISTORM_NO_TEXT
  using font = void;
#else
class font {
  /// Container class to hold a font object and metadata about it
public:
  struct glyph {
    /// Container for the dimensions of glyph rectangles and their texcoords
    friend class font;
    #ifdef GUISTORM_NO_UTF
      char charcode = '\0';                                                     // what character this glyph represents (ascii)
    #else
      char32_t charcode = U'\0';                                                // what character this glyph represents (unicode)
    #endif // GUISTORM_NO_UTF
    bool is_blank = false;                                                      // for spaces and other invisible horizontal whitespace glyphs
    bool linebreak = false;                                                     // whether to add a line break after this glyph
    coordtype offset;                                                           // lower-left corner of the quad
    coordtype size;                                                             // size of the quad
    coordtype texcoord0;                                                        // texcoord of the lower left corner in the texture atlas
    coordtype texcoord1;                                                        // texcoord of the upper right corner in the texture atlas
    coordtype advance;                                                          // how far this moves the cursor forward after it's placed
  protected:
    #ifdef GUISTORM_NO_UTF
      std::unordered_map<char, GLfloat> kerning;                                // map of kerning for this glyph by preceding ascii character
    #else
      std::unordered_map<char32_t, GLfloat> kerning;                            // map of kerning for this glyph by preceding unicode character
    #endif // GUISTORM_NO_UTF
  public:
    #ifdef GUISTORM_NO_UTF
      GLfloat get_kerning(char charcode_last) const;
    #else
      GLfloat get_kerning(char32_t charcode_last) const;
    #endif // GUISTORM_NO_UTF
  };
  struct word {
    /// Container for the glyphs that make up a single word of text
    std::vector<std::shared_ptr<font::glyph>> glyphs;                           // the glyphs in this word, including a trailing space, if used
    bool linebreak = false;                                                     // whether to add a line break after this word
    GLfloat length() const;
  };
  struct line {
    /// Container for the words that make up a single line of text
    std::vector<word> words;                                                    // the words making up this line
    coordtype size;                                                             // 2D size of this line
    GLfloat spacing = 0.0f;                                                     // additional spacing between words in this line, used in justification
    bool linebreak = false;                                                     // whether this line breaks specifically before it wraps
    GLfloat length() const;
  };

private:
  gui *parent_gui = nullptr;
  #ifdef GUISTORM_NO_UTF
    std::unordered_map<char, std::shared_ptr<glyph>> glyphs;                    // library of ascii glyphs
  #else
    std::unordered_map<char32_t, std::shared_ptr<glyph>> glyphs;                // library of unicode glyphs
  #endif // GUISTORM_NO_UTF
  mutable std::mutex glyph_map_mutex;                                           // mutex to prevent glyphs being modified while being read
public:
  std::string name;
  std::string_view buffer;                                                      // offset and size in memory of the raw font data
  float font_size = 0;                                                          // font size to load this font at, in points
  GLfloat metrics_ascender  = 0.0;
  GLfloat metrics_descender = 0.0;
  GLfloat metrics_height    = 0.0;
  GLfloat metrics_linegap   = 0.0;

  #ifdef GUISTORM_NO_UTF
    std::string charcodes;                                                      // string containing all the ascii glyphs to be generated for this font
  #else
    std::u32string charcodes;                                                   // string containing all the unicode glyphs to be generated for this font
  #endif // GUISTORM_NO_UTF
  bool force_autohint           = false;                                        // whether to force font hinting - can introduce unnecessary blur
  bool suppress_horizontal_hint = true;                                         // whether to suppress horizontal hints for better high-res rendering
  bool suppress_autohunt        = false;                                        // whether to disable autohint (ignored if force_autohint is on)
  bool suppress_hinting         = false;                                        // whether to disable font hinting entirely (ignored if force_autohint is on)
private:
  static GLfloat constexpr horizontal_hint_suppression = 64.0f;
  static GLfloat constexpr hres = 64.0f;                                        // from #define HRES 64 - Freetype uses 1/64th of a point scale

public:
  font(gui *parent_gui,
       std::string const &name,
       std::string_view buffer,
       float font_size,
       #ifdef GUISTORM_NO_UTF
         std::string const &charcodes_to_load = "",
       #else
        std::u32string const &charcodes_to_load = U"",
       #endif // GUISTORM_NO_UTF
       bool suppress_horizontal_hint = true);
  ~font();

  bool load_if_needed(freetypeglxx::TextureAtlas *font_atlas);
  bool load(freetypeglxx::TextureAtlas *font_atlas);
  #ifdef GUISTORM_NO_UTF
    bool load_glyphs(freetypeglxx::TextureAtlas *font_atlas, FT_Face const &face, std::string const &charcodes);
    bool load_glyph( freetypeglxx::TextureAtlas *font_atlas, FT_Face const &face, char charcode);
  #else
    bool load_glyphs(freetypeglxx::TextureAtlas *font_atlas, FT_Face const &face, std::u32string const &charcodes);
    bool load_glyph( freetypeglxx::TextureAtlas *font_atlas, FT_Face const &face, char32_t charcode);
  #endif // GUISTORM_NO_UTF
  void unload();

  void update_kerning(FT_Face const &face);
  void update_kerning(FT_Face const &face, glyph &first);
  void update_kerning(FT_Face const &face, glyph &first, glyph const &second);

  #ifdef GUISTORM_NO_UTF
    std::shared_ptr<font::glyph> const getglyph(char charcode);
  #else
    std::shared_ptr<font::glyph> const getglyph(char32_t charcode);
  #endif // GUISTORM_NO_UTF
};
#endif // GUISTORM_NO_TEXT

}

#endif // GUISTORM_FONT_H_INCLUDED
