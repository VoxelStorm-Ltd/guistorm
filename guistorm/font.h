#ifndef GUISTORM_FONT_H_INCLUDED
#define GUISTORM_FONT_H_INCLUDED

#include <string>
#include <vector>
#include <unordered_map>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype-gl++/freetype-gl++.hpp>
#include "types.h"

namespace guistorm {

class gui;

class font {
  /// Container class to hold a font object and metadata about it
public:
  struct glyph {
    /// Container for the dimensions of glyph rectangles and their texcoords
    friend class font;
    char32_t charcode = U'\0';                                      // what character this glyph represents
    bool is_blank = false;                                          // for spaces and other invisible horizontal whitespace glyphs
    bool linebreak = false;                                         // whether to add a line break after this glyph
    coordtype offset;                                               // lower-left corner of the quad
    coordtype size;                                                 // size of the quad
    coordtype texcoord0;                                            // texcoord of the lower left corner in the texture atlas
    coordtype texcoord1;                                            // texcoord of the upper right corner in the texture atlas
    coordtype advance;                                              // how far this moves the cursor forward after it's placed
  protected:
    std::unordered_map<char32_t, GLfloat> kerning;                  // map of kerning for this glyph by preceding character
  public:
    GLfloat get_kerning(char32_t charcode_last) const;
  };
  struct word {
    /// Container for the glyphs that make up a single word of text
    std::vector<glyph const*> glyphs;                               // the glyphs in this word, including a trailing space, if used
    bool linebreak = false;                                         // whether to add a line break after this word
    GLfloat length() const;
  };
  struct line {
    /// Container for the words that make up a single line of text
    std::vector<word> words;                                        // the words making up this line
    coordtype size;                                                 // 2D size of this line
    GLfloat spacing = 0.0f;                                         // additional spacing between words in this line, used in justification
    bool linebreak = false;                                         // whether this line breaks specifically before it wraps
    GLfloat length() const;
  };

private:
  gui *parent_gui = nullptr;
  std::unordered_map<char32_t, glyph*> glyphs;                      // library of glyphs
public:
  std::string name;
  const unsigned char*  memory_offset = 0;                          // offset in memory of the raw font data
  size_t                memory_size   = 0;                          // size in memory of the raw font data
  float                 font_size     = 0;                          // font size to load this font at, in points
  GLfloat metrics_ascender  = 0.0;
  GLfloat metrics_descender = 0.0;
  GLfloat metrics_height    = 0.0;
  GLfloat metrics_linegap   = 0.0;

  std::u32string charcodes;                                         // string containing all the glyphs to be generated for this font
  bool force_autohint           = false;                            // whether to force font hinting - can introduce unnecessary blur
  bool suppress_horizontal_hint = true;                             // whether to suppress horizontal hints for better high-res rendering
  bool suppress_autohunt        = false;                            // whether to disable autohint (ignored if force_autohint is on)
  bool suppress_hinting         = false;                            // whether to disable font hinting entirely (ignored if force_autohint is on)
private:
  static GLfloat constexpr horizontal_hint_suppression = 64.0f;
  static GLfloat constexpr hres = 64.0f;                            // from #define HRES 64 - Freetype uses 1/64th of a point scale

public:
  font(gui *parent_gui,
       std::string const &name,
       unsigned char const *memory_offset,
       size_t memory_size,
       float font_size,
       std::u32string const &charcodes_to_load = U"",
       bool suppress_horizontal_hint = true);
  ~font();

  bool load_if_needed(freetypeglxx::TextureAtlas *font_atlas);
  bool load(freetypeglxx::TextureAtlas *font_atlas);
  bool load_glyphs(freetypeglxx::TextureAtlas *font_atlas, FT_Face const &face, std::u32string const &charcodes);
  bool load_glyph( freetypeglxx::TextureAtlas *font_atlas, FT_Face const &face, char32_t charcode);
  void unload();

  void update_kerning(FT_Face const &face);
  void update_kerning(FT_Face const &face, glyph &first);
  void update_kerning(FT_Face const &face, glyph &first, glyph const &second);

  glyph const *getglyph(char32_t charcode);
};

}

#endif // GUISTORM_FONT_H_INCLUDED
