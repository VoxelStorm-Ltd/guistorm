#include "gui.h"
#include <algorithm>
#include <boost/algorithm/clamp.hpp>
#include <freetype-gl/texture-atlas.h>
#include "blob_loader.h"
#include "shader_load.h"
#include "base.h"
#include "font.h"
#include "lineshape.h"

namespace guistorm {

GLuint gui::shader = 0;
GLfloat constexpr gui::dpi_default;             // static
GLfloat constexpr gui::dpi_min;
GLfloat constexpr gui::dpi_max;

gui::gui() {
  /// Default constructor
}

gui::~gui() {
  /// Default destructor
  clear();
  destroy();
  for(auto &it : fonts) {
    delete it;
  }
  fonts.clear();
  delete cursor;
}

void gui::init() {
  /// Wrapper function to call all initialisations
  init_buffer();
  load_shader();
  load_fonts();
}

void gui::destroy() {
  /// Wrapper function to call all cleanup functions in preparation for exit or context switch
  destroy_buffer();
  destroy_shader();
  destroy_fonts();
}

void gui::init_buffer() {
  /// Generate the buffers for this object
  std::cout << "GUIStorm: Initialising " << elements.size() << " top level buffers..." << std::endl;
  for(auto & element : elements) {
    element->init_buffer();
  }
}
void gui::destroy_buffer() {
  /// Clean up the buffers in preparation for exit or context switch
  container::destroy_buffer();
}

void gui::load_shader() {
  /// Load and initialise the gui shader
  if(shader != 0) {
    return;                   // shader already initialised elsewhere
  }
  std::cout << "GUIStorm: ";
  shader = shader_load(std::string(R"(#version 120
                                      #pragma optimize(on)
                                      #pragma debug(off)

                                      attribute vec4 coords;    // we only input a vec3, so w defaults to 1.0
                                      attribute vec2 texcoords;

                                      varying vec2 texcoords_frag;

                                      void main() {
                                        texcoords_frag = texcoords;
                                        gl_Position = coords;
                                      }

                                   )"),
                       std::string(R"(#version 120
                                      #pragma optimize(on)
                                      #pragma debug(off)

                                      uniform vec4 colour;
                                      uniform sampler2D texture;

                                      varying vec2 texcoords_frag;

                                      void main() {
                                        float a = texture2D(texture, texcoords_frag).a;
                                        gl_FragColor = vec4(colour.rgb, colour.a * a);
                                      }
                                   )"));
  if(shader == GL_FALSE) {
    std::cout << "GUIStorm: ERROR: " << __PRETTY_FUNCTION__ << ": Failed to load shaders, exiting." << std::endl;
    exit(EXIT_FAILURE);
  }
  // cache attribute and uniform indices
  attrib_coords    = glGetAttribLocation(shader, "coords");
  attrib_texcoords = glGetAttribLocation(shader, "texcoords");
  uniform_colour   = glGetUniformLocation(shader, "colour");
}

void gui::destroy_shader() {
  /// Clean up the shader in preparation for exit or context switch
  glDeleteProgram(shader);
  shader = 0;
}

void gui::load_fonts() {
  /// Initialise the font atlas and any font associated objects
  /// Note: TextureAtlas depth == 1 uses format GL_RED by default which is not available on older hardware, so we need to upload manually in those cases
  Vector2<size_t> newsize(256, 256);
  bool atlas_complete;
  do {
    atlas_complete = true;
    delete font_atlas;
    font_atlas = new freetypeglxx::TextureAtlas(newsize.x, newsize.y, 1);
    std::cout << "GUIStorm: Loading " << fonts.size() << " fonts to " << font_atlas->width() << "x" << font_atlas->height() << " atlas..." << std::endl;
    for(auto const &thisfont : fonts) {
      atlas_complete = thisfont->load(font_atlas);
      if(!atlas_complete) {                      // attempt to scale the texture until we reach opengl texture max size
        std::cout << "GUIStorm: Texture atlas full (no room for " << thisfont->name << " size " << thisfont->font_size << "), growing atlas..." << std::endl;
        thisfont->unload();
        newsize *= 2;
        GLint maxtexture;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxtexture);
        if(newsize.x > static_cast<unsigned int>(maxtexture)) {
          std::cout << "GUIStorm: ERROR: would need to scale atlas past max texture size of " << maxtexture << "x" << maxtexture << ", abandoning!" << std::endl;
          return;
        }
        break;
      }
    }
  } while(!atlas_complete);
  upload_fonts();            // upload manually since we've reimplemented loadGlyphs' uploader and so not using font_atlas->Upload()
}

void gui::upload_fonts() {
  /// Manually upload the texture as GL_ALPHA instead of not-always-supported GL_RED which is default in freetype-gl
  texture_atlas_t *atlas_self = static_cast<texture_atlas_t*>(font_atlas->RawGet());
  if(!font_atlas->id()) {                                               // if no texture has been generated, then generate one ourselves
    glGenTextures(1, &atlas_self->id);
  }
  glBindTexture(GL_TEXTURE_2D, font_atlas->id());
  if(font_atlas_filtering) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  }
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, font_atlas->width(), font_atlas->height(), 0, GL_ALPHA, GL_UNSIGNED_BYTE, atlas_self->data);

  // set the 1,0 to 1,1 texels of the font atlas texture to a 0.0-1.0 alpha gradient to use the shader for solid objects without texture switching
  unsigned int const strip_height = 2;
  GLfloat data[strip_height][font_atlas->width()];
  for(unsigned int y = 0; y != strip_height; ++y) {
    for(unsigned int x = 0; x != font_atlas->width(); ++x) {
      data[y][x] = static_cast<GLfloat>(x) / (font_atlas->width() - 1);   // produce a gradient from 0 to 1 in unsigned byte form
    }
  }
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, font_atlas->height() - strip_height, font_atlas->width(), strip_height, GL_ALPHA, GL_FLOAT, data);
  glBindTexture(GL_TEXTURE_2D, 0);
  std::cout << "GUIStorm: Font atlas uploaded, " << (font_atlas->width() * font_atlas->height()) / 1024 << "KB, id=" << font_atlas->id() << std::endl;
}

void gui::destroy_fonts() {
  /// Clean up the font atlas in preparation for exit or context switch
  delete font_atlas;
  font_atlas = nullptr;
}

void gui::refresh() {
  /// Re-create the buffers of all elements in this gui
  container::refresh();
  if(cursor) {
    cursor->refresh();
  }
  picked_element = get_picked(cursor_position);       // traverse the tree to update the currently picked element
}

void gui::render() {
  /// Render every visible element in the gui
  if(__builtin_expect(shader == 0, 0)) {  // if the shader hasn't been loaded yet (unlikely)
    std::cout << "WARNING: shader had not been pre-loaded before gui::render called" << std::endl;
    load_shader();
  }
  glDisable(GL_DEPTH_TEST);
  glUseProgram(shader);
  glEnableVertexAttribArray(attrib_coords);
  glEnableVertexAttribArray(attrib_texcoords);
  glBindTexture(GL_TEXTURE_2D, font_atlas->id());

  container::render();

  if(cursor) {
    cursor->render();
  }

  glBindBuffer(GL_ARRAY_BUFFER,         0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glUseProgram(0);
  glDisableVertexAttribArray(attrib_coords);
  glDisableVertexAttribArray(attrib_texcoords);
  glBindTexture(GL_TEXTURE_2D, 0);
  glEnable(GL_DEPTH_TEST);

  mouse_released = false;
  if(mouse_pressed) {
    ++mouse_pressed_frames;         // keep track of how long the mouse has been pressed
  } else {
    if(mouse_pressed_frames != 0) {
      mouse_released = true;        // create a flag for one frame when the mouse is released
    }
    mouse_pressed_frames = 0;
  }
}

void gui::add_to_gui(base *element) {
  /// Add the element to this gui
  element->parent_gui = this;
}

void gui::add_font(std::string const &name,
                   const unsigned char* memory_offset,
                   size_t memory_size,
                   unsigned int font_size,
                   std::string const &glyphs_to_load) {
  /// Font factory that sets up font ownership with this GUI
  fonts.emplace_back(new font(this, name, memory_offset, memory_size, font_size, glyphs_to_load));
}
void gui::add_font(font *thisfont) {
  /// Take ownership of an existing font
  /// NOTE: guistorm will now free this font, do not try to delete it manually
  fonts.emplace_back(thisfont);
}

void gui::set_windowsize(coordtype const &new_windowsize) {
  /// Cache the window size and refresh buffers if it's changed
  #ifdef DEBUG_GUISTORM
    std::cout << "GUIStorm: Window resized to " << new_windowsize << std::endl;
  #endif // DEBUG_GUISTORM
  if(windowsize == new_windowsize) {
    windowsize = new_windowsize;
  } else {
    windowsize = new_windowsize;
    // update all buffers
    if(glfwGetCurrentContext() != NULL) {     // make sure we're in a valid opengl context before we try to refresh
      refresh();
    }
  }
}

font *gui::get_font_by_size(float size) {
  /// Attempt to find a font of the specified size, and return nullptr if not found
  for(auto const &f : fonts) {
    if(f->font_size == size) {
      return f;
    }
  }
  return nullptr;
}
font *gui::get_font_by_size_or_nearest(float size) {
  /// Attempt to find a font of the specified size, and return the nearest in size, or nullptr if no others found
  font *f = nullptr;
  for(auto const &thisfont : fonts) {
    if(f) {
      if(std::abs(thisfont->font_size - size) < std::abs(f->font_size - size)) {      // compare to get the smallest difference in font size
        f = thisfont;
      }
    } else {
      f = thisfont;
    }
  }
  return f;
}
font *gui::get_font_by_size_or_smaller(float size) {
  /// Attempt to find a font of the specified size, and return the next smallest in size, or nullptr if none smaller found
  font *f = nullptr;
  for(auto const &thisfont : fonts) {
    if(thisfont->font_size <= size) {                     // filter for same size or smaller fonts only
      if(f) {
        if(thisfont->font_size > f->font_size) {          // compare to get the biggest font size of the filtered fonts
          f = thisfont;
        }
      } else {
        f = thisfont;
      }
    }
  }
  return f;
}
font *gui::get_font_by_size_or_bigger(float size) {
  /// Attempt to find a font of the specified size, and return the next biggest in size, or nullptr if none bigger found
  font *f = nullptr;
  for(auto const &thisfont : fonts) {
    if(thisfont->font_size >= size) {                     // filter for same size or bigger fonts only
      if(f) {
        if(thisfont->font_size < f->font_size) {          // compare to get the smalleset font size of the filtered fonts
          f = thisfont;
        }
      } else {
        f = thisfont;
      }
    }
  }
  return f;
}


GLfloat gui::get_dpi() const {
  return dpi;
}
GLfloat gui::get_dpi_scale() const {
  return dpi_scale;
}
void gui::set_dpi(GLfloat newdpi) {
  /// Update dpi and update the cached value of dpi scale
  dpi = boost::algorithm::clamp(newdpi, dpi_min, dpi_max);
  dpi_scale = newdpi / dpi_default;
}
void gui::set_dpi_scale(GLfloat newscale) {
  /// Update desired dpi scale factor and calculate new dpi from that
  set_dpi(dpi_default * newscale);
}

void gui::set_cursor_position(coordtype const &new_cursor_position) {
  /// Update the cursor position
  cursor_position = new_cursor_position;
  if(cursor) {
    cursor->set_position_nodpiscale(cursor_position); // don't call the dpi scaler function
    cursor->refresh();
  }
  picked_element = get_picked(cursor_position);       // traverse the tree to update the currently picked element
}

void gui::set_mouse_pressed() {
  /// Tell the gui the mouse has been pressed
  mouse_pressed = true;
}
void gui::set_mouse_released() {
  /// Tell the gui the mouse is being released
  mouse_pressed = false;
}

coordtype gui::coord_transform(coordtype const &coord) {
  /// Helper to transform screen coordinates into screen space suitable for feeding to the shader without further transformation
  return coordtype((coord.x * 2 / windowsize.x) - 1.0,
                   (coord.y * 2 / windowsize.y) - 1.0);
}

}
