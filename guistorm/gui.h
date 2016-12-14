#ifndef GUISTORM_GUI_H_INCLUDED
#define GUISTORM_GUI_H_INCLUDED

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#ifndef GUISTORM_NO_TEXT
  #include <freetype-gl++/texture-atlas.hpp>
#endif // GUISTORM_NO_TEXT
#include <guistorm/types.h>
#include <guistorm/container.h>
#include <guistorm/font.h>

namespace guistorm {

class lineshape;
class input_text;

class gui : public container {
  friend class base;
  friend class input_text;
  friend class line;
  friend class lineshape;
  friend class progressbar;
  friend class graph_line;
  friend class graph_ringbuffer_line;
protected:
  static GLuint shader;                                                         // the shader for rendering all gui elements
  #ifndef GUISTORM_NO_TEXT
    freetypeglxx::TextureAtlas *font_atlas = nullptr;                           // texture atlas containing all font glyphs we use
  #endif // GUISTORM_NO_TEXT
public:
  bool font_atlas_filtering = true;                                             // whether to filter the font atlas linearly or use nearest neighbour - for subpixel offsets
  #ifndef GUISTORM_NO_TEXT
    std::vector<font*> fonts;                                                   // the list of fonts we contain
    font *font_default = nullptr;                                               // which font to recommend as default to child objects
  #endif // GUISTORM_NO_TEXT
protected:
  // per-vertex attribute indices
  GLuint attrib_coords    = 0;
  GLuint attrib_texcoords = 0;
  GLuint uniform_colour   = 0;

public:
  static GLfloat constexpr dpi_default = 72.0;                                  // standard pixels per inch
  static GLfloat constexpr dpi_min     = 18.0;                                  // minimum allowed dpi value
  static GLfloat constexpr dpi_max     = 500.0;                                 // maximum allowed dpi value
private:
  GLfloat dpi = 72.0;                                                           // the dots per inch of the display - used to scale and all elements
  GLfloat dpi_scale = 1.0;                                                      // size multiplier to scale gui layout and fonts for the screen
public:
  coordtype windowsize;                                                         // cached value for the size of the window
  coordtype cursor_position;                                                    // cached value for the window position of the cursor
  bool mouse_pressed = false;                                                   // whether we're holding down the mousebutton
  bool mouse_released = false;                                                  // whether the mouse was being held and has been released this frame
  unsigned int mouse_pressed_frames = 0;                                        // how long the mouse has been held down, in frames

  base *picked_element = nullptr;                                               // what element we're currently hovering over, if any

  base *cursor = nullptr;                                                       // what entity is acting as this gui's current cursor, if any

  #ifndef GUISTORM_NO_TEXT
    // input field management
    std::function<void(input_text&)> function_select_input   = [](input_text &this_input __attribute__((__unused__))){}; // what to call on a selected input field, for binding text input callbacks etc
    std::function<void(input_text&)> function_deselect_input = [](input_text &this_input __attribute__((__unused__))){}; // what to call on a deselected input field, for unbinding etc
    input_text *current_input_field = nullptr;                                  // what input field we have selected, if any
  #endif // GUISTORM_NO_TEXT

public:
  gui();
  ~gui();

  void init();
  void destroy();
  void init_buffer();
  virtual void destroy_buffer() override final;
  void load_shader();
  void destroy_shader();
  #ifndef GUISTORM_NO_TEXT
    void load_fonts();
    void upload_fonts();
    void destroy_fonts();
  #endif // GUISTORM_NO_TEXT
  void refresh() override final;

  void render() override final;

  void add_to_gui(base *element) override final;
  #ifndef GUISTORM_NO_TEXT
    void add_font(std::string const &name,
                  unsigned char const *memory_offset,
                  size_t memory_size,
                  float font_size,
                  #ifdef GUISTORM_NO_UTF
                    std::string const &glyphs_to_load = ""
                  #else
                    std::u32string const &glyphs_to_load = U""
                  #endif // GUISTORM_NO_UTF
                  );
    void add_font(font *thisfont);
    void clear_fonts();
    #ifdef DEBUG_GUISTORM
      font *get_font_by_size(           float size);
      font *get_font_by_size_or_nearest(float size);
      font *get_font_by_size_or_smaller(float size);
      font *get_font_by_size_or_bigger( float size);
    #else
      font *get_font_by_size(           float size) __attribute__((__pure__));
      font *get_font_by_size_or_nearest(float size) __attribute__((__pure__));
      font *get_font_by_size_or_smaller(float size) __attribute__((__pure__));
      font *get_font_by_size_or_bigger( float size) __attribute__((__pure__));
    #endif // DEBUG_GUISTORM
  #endif // GUISTORM_NO_TEXT

  // environment and input control
  void set_windowsize(coordtype const &new_windowsize);
  GLfloat get_dpi() const __attribute__((__pure__));
  GLfloat get_dpi_scale() const __attribute__((__pure__));
  void set_dpi(GLfloat newdpi);
  void set_dpi_scale(GLfloat newscale);
  void set_cursor_position(coordtype const &new_cursor_position);
  void update_cursor_pick();
