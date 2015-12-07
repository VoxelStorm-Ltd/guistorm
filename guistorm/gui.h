#ifndef GUISTORM_H_INCLUDED
#define GUISTORM_H_INCLUDED

/// Extensible, fast, customisable window and widget display toolkit version 2.0
/// by Eugene Hopkinson (SlowRiot) for VoxelStorm 2014
///
/// Defines: GUISTORM_AVOIDQUADS - use triangles instead of deprecated GL_QUADS primitives even if quads are cheaper
///          DEBUG_GUISTORM - draw outlines of hidden gui elements and show advanced debugging messages

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <freetype-gl++/texture-atlas.hpp>
#include <guistorm/types.h>
#include <guistorm/container.h>
#include <guistorm/font.h>

namespace guistorm {

class lineshape;

class gui : public container {
  friend class base;
  friend class line;
  friend class lineshape;
  friend class progressbar;
  friend class graph_line;
protected:
  static GLuint shader;                                     // the shader for rendering all gui elements
  freetypeglxx::TextureAtlas *font_atlas = nullptr;         // texture atlas containing all font glyphs we use
public:
  bool font_atlas_filtering = true;                         // whether to filter the font atlas linearly or use nearest neighbour - for subpixel offsets
  std::vector<font*> fonts;                                 // the list of fonts we contain
  font *font_default = nullptr;                             // which font to recommend as default to child objects
protected:
  // per-vertex attribute indices
  GLuint attrib_coords    = 0;
  GLuint attrib_texcoords = 0;
  GLuint uniform_colour   = 0;

public:
  static GLfloat constexpr dpi_default = 72.0;              // standard pixels per inch
  static GLfloat constexpr dpi_min     = 18.0;              // minimum allowed dpi value
  static GLfloat constexpr dpi_max     = 500.0;             // maximum allowed dpi value
private:
  GLfloat dpi = 72.0;                                       // the dots per inch of the display - used to scale and all elements
  GLfloat dpi_scale = 1.0;                                  // size multiplier to scale gui layout and fonts for the screen
public:
  coordtype windowsize;                                     // cached value for the size of the window
  coordtype cursor_position;                                // cached value for the window position of the cursor
  bool mouse_pressed = false;                               // whether we're holding down the mousebutton
  bool mouse_released = false;                              // whether the mouse was being held and has been released this frame
  unsigned int mouse_pressed_frames = 0;                    // how long the mouse has been held down, in frames

  base *picked_element = nullptr;                           // what element we're currently hovering over, if any

  base *cursor = nullptr;                                   // what entity is acting as this gui's current cursor, if any

public:
  gui();
  ~gui();

  void init();
  void destroy();
  void init_buffer();
  virtual void destroy_buffer() override final;
  void load_shader();
  void destroy_shader();
  void load_fonts();
  void upload_fonts();
  void destroy_fonts();
  void refresh() override final;

  void render() override final;

  void add_to_gui(base *element) override final;
  void add_font(std::string const &name,
                const unsigned char* memory_offset,
                size_t memory_size,
                float font_size,
                std::string const &glyphs_to_load = "");
  void add_font(font *thisfont);
  void clear_fonts();
  font *get_font_by_size(           float size)
  #ifndef DEBUG_GUISTORM
    __attribute__((__pure__))
  #endif // DEBUG_GUISTORM
  ;
  font *get_font_by_size_or_nearest(float size)
  #ifndef DEBUG_GUISTORM
    __attribute__((__pure__))
  #endif // DEBUG_GUISTORM
  ;
  font *get_font_by_size_or_smaller(float size)
  #ifndef DEBUG_GUISTORM
    __attribute__((__pure__))
  #endif // DEBUG_GUISTORM
  ;
  font *get_font_by_size_or_bigger( float size)
  #ifndef DEBUG_GUISTORM
    __attribute__((__pure__))
  #endif // DEBUG_GUISTORM
  ;

  // environment and input control
  void set_windowsize(coordtype const &new_windowsize);
  GLfloat get_dpi() const __attribute__((__pure__));
  GLfloat get_dpi_scale() const __attribute__((__pure__));
  void set_dpi(GLfloat newdpi);
  void set_dpi_scale(GLfloat newscale);
  void set_cursor_position(coordtype const &new_cursor_position);
  void update_cursor_pick();
  void set_mouse_pressed();
  void set_mouse_released();

  // helpers
  coordtype coord_transform(coordtype const &coord);
};

}

#endif // GUISTORM_H_INCLUDED
