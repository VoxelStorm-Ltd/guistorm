#include "gui.h"
#include <iostream>
#ifndef GUISTORM_NO_TEXT
  #include <freetype-gl/texture-atlas.h>
#endif // GUISTORM_NO_TEXT
#include "blob_loader.h"
#include "cast_if_required.h"
#include "shader_load.h"
#include "rounding.h"
#ifdef GUISTORM_NO_TEXT
  #include "base.h"
#else
  #include "input_text.h"
#endif // GUISTORM_NO_TEXT

namespace guistorm {

GLuint gui::shader = 0;
GLfloat constexpr gui::dpi_default;
GLfloat constexpr gui::dpi_min;
GLfloat constexpr gui::dpi_max;

gui::gui() {
  /// Default constructor
}

gui::~gui() {
  /// Default destructor
  clear();
  #ifndef GUISTORM_NO_TEXT
    clear_fonts();
  #endif // GUISTORM_NO_TEXT
  destroy();
}

void gui::init() {
  /// Wrapper function to call all initialisations
  init_buffer();
  load_shader();
  #ifndef GUISTORM_NO_TEXT
    load_fonts();
  #endif // GUISTORM_NO_TEXT
}

void gui::destroy() {
  /// Wrapper function to call all cleanup functions in preparation for exit or context switch
  destroy_buffer();
  destroy_shader();
  #ifndef GUISTORM_NO_TEXT
    destroy_fonts();
  #endif // GUISTORM_NO_TEXT
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
    return;                                                                     // shader already initialised elsewhere
  }
  std::cout << "GUIStorm: ";
  shader = shader_load(std::string(R"(#version 120
                                      #pragma optimize(on)
                                      #pragma debug(off)

                                      attribute vec4 coords;                    // we only input a vec3, so w defaults to 1.0
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
  if(shader == 0) {
    return;
  }
  glDeleteProgram(shader);
  shader = 0;
}

#ifndef GUISTORM_NO_TEXT
void gui::load_fonts() {
  /// Initialise the font atlas and any font associated objects
  /// Note: TextureAtlas depth == 1 uses format GL_RED by default which is not available on older hardware, so we need to upload manually in those cases
  vec2<size_t> newsize(256, 256);
  bool atlas_complete;
  do {
    atlas_complete = true;
    delete font_atlas;
    font_atlas = new freetypeglxx::TextureAtlas(newsize.x, newsize.y, 1);
    std::cout << "GUIStorm: Loading " << fonts.size() << " fonts to " << font_atlas->width() << "x" << font_atlas->height() << " atlas..." << std::endl;
    for(auto const &thisfont : fonts) {
      atlas_complete = thisfont->load(font_atlas);
      if(!atlas_complete) {                                                     // attempt to scale the texture until we reach opengl texture max size
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
  upload_fonts();                                                               // upload manually since we've reimplemented loadGlyphs' uploader and so not using font_atlas->Upload()
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstack-usage="
void gui::upload_fonts() {
  /// Manually upload the texture as GL_ALPHA instead of not-always-supported GL_RED which is default in freetype-gl
  texture_atlas_t *atlas_self = static_cast<texture_atlas_t*>(font_atlas->RawGet());
  if(!font_atlas->id()) {                                                       // if no texture has been generated, then generate one ourselves
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
  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_ALPHA,
               cast_if_required<GLsizei>(font_atlas->width()),
               cast_if_required<GLsizei>(font_atlas->height()),
               0,
               GL_ALPHA,
               GL_UNSIGNED_BYTE,
               atlas_self->data);

  // set the 1,0 to 1,1 texels of the font atlas texture to a 0.0-1.0 alpha gradient to use the shader for solid objects without texture switching
  constexpr GLsizei strip_height = 2;
  GLfloat data[strip_height][font_atlas->width()];
  for(GLsizei y = 0; y != strip_height; ++y) {
    for(GLsizei x = 0; x != cast_if_required<GLsizei>(font_atlas->width()); ++x) {
      data[y][x] = static_cast<GLfloat>(x) / static_cast<GLfloat>(font_atlas->width() - 1); // produce a gradient from 0 to 1 in unsigned byte form
    }
  }
  glTexSubImage2D(GL_TEXTURE_2D,
                  0,
                  0,
                  cast_if_required<GLsizei>(font_atlas->height()) - strip_height,
                  cast_if_required<GLsizei>(font_atlas->width()),
                  strip_height,
                  GL_ALPHA,
                  GL_FLOAT,
                  data);
  glBindTexture(GL_TEXTURE_2D, 0);
  std::cout << "GUIStorm: Font atlas uploaded, " << (font_atlas->width() * font_atlas->height()) / 1024 << "KB, id=" << font_atlas->id() << std::endl;
}
#pragma GCC diagnostic pop

void gui::destroy_fonts() {
  /// Clean up the font atlas in preparation for exit or context switch
  for(auto f : fonts) {
    f->unload();
  }
  delete font_atlas;
  font_atlas = nullptr;
}
#endif // GUISTORM_NO_TEXT

void gui::refresh() {
  /// Re-create the buffers of all elements in this gui
  container::refresh();
  picked_element = get_picked(cursor_position);                                 // traverse the tree to update the currently picked element
}

void gui::render() {
  /// Render every visible element in the gui
  if(__builtin_expect(shader == 0, 0)) {                                        // if the shader hasn't been loaded yet (unlikely)
    std::cout << "WARNING: shader had not been pre-loaded before gui::render called" << std::endl;
    load_shader();
  }
  glDisable(GL_DEPTH_TEST);
  glUseProgram(shader);
  glEnableVertexAttribArray(attrib_coords);
  glEnableVertexAttribArray(attrib_texcoords);
  #ifndef GUISTORM_NO_TEXT
    glBindTexture(GL_TEXTURE_2D, font_atlas->id());
  #endif // GUISTORM_NO_TEXT

  container::render();

  glDisableVertexAttribArray(attrib_coords);
  glDisableVertexAttribArray(attrib_texcoords);
  #ifdef GUISTORM_UNBIND
    glBindBuffer(GL_ARRAY_BUFFER,         0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(0);
    #ifndef GUISTORM_NO_TEXT
      glBindTexture(GL_TEXTURE_2D, 0);
    #endif // GUISTORM_NO_TEXT
  #endif // GUISTORM_UNBIND
  glEnable(GL_DEPTH_TEST);

  mouse_released = false;
  if(mouse_pressed) {
    ++mouse_pressed_frames;                                                     // keep track of how long the mouse has been pressed
  } else {
    if(mouse_pressed_frames != 0) {
      mouse_released = true;                                                    // create a flag for one frame when the mouse is released
    }
    mouse_pressed_frames = 0;
  }
}

void gui::add_to_gui(base *element) {
  /// Add the element to this gui
  element->parent_gui = this;
}

#ifndef GUISTORM_NO_TEXT
void gui::add_font(std::string const &name,
                   std::string_view buffer,
                   float font_size,
                   #ifdef GUISTORM_NO_UTF
                     std::string const &glyphs_to_load
                   #else
                     std::u32string const &glyphs_to_load
                   #endif // GUISTORM_NO_UTF
                   ) {
  /// Font factory that sets up font ownership with this GUI
  fonts.emplace_back(new font(this, name, buffer, font_size, glyphs_to_load));
  #ifdef DEBUG_GUISTORM
    std::cout << "GUIStorm: added font " << name << " size " << font_size << ", " << fonts.size() << " total" << std::endl;
  #endif // DEBUG_GUISTORM
}
void gui::add_font(font *thisfont) {
  /// Take ownership of an existing font
  /// NOTE: guistorm will now free this font, do not try to delete it manually
  fonts.emplace_back(thisfont);
}
void gui::clear_fonts() {
  /// Clear the loaded fonts list
  #ifdef DEBUG_GUISTORM
    std::cout << "GUIStorm: clearing " << fonts.size() << " fonts" << std::endl;
  #endif // DEBUG_GUISTORM
  for(auto &it : fonts) {
    delete it;
  }
  fonts.clear();
}

font *gui::get_font_by_size(float size) {
  /// Attempt to find a font of the specified size, and return nullptr if not found
  #ifdef DEBUG_GUISTORM
    std::cout << "GUIStorm: requested font size " << size << std::endl;
  #endif // DEBUG_GUISTORM
  for(auto const &f : fonts) {
    if(f->font_size == size) {
      #ifdef DEBUG_GUISTORM
        std::cout << "GUIStorm: found font size " << size << " (" << f->name << ")" << std::endl;
      #endif // DEBUG_GUISTORM
      return f;
    }
  }
  #ifdef DEBUG_GUISTORM
    std::cout << "GUIStorm: unable to find font size " << size << ", using default" << std::endl;
  #endif // DEBUG_GUISTORM
  return nullptr;
}
font *gui::get_font_by_size_or_nearest(float size) {
  /// Attempt to find a font of the specified size, and return the nearest in size, or nullptr if no others found
  #ifdef DEBUG_GUISTORM
    std::cout << "GUIStorm: requested font nearest to size " << size << std::endl;
  #endif // DEBUG_GUISTORM
  font *f = nullptr;
  for(auto const &thisfont : fonts) {
    if(f) {
      if(std::abs(thisfont->font_size - size) < std::abs(f->font_size - size)) { // compare to get the smallest difference in font size
        f = thisfont;
        #ifdef DEBUG_GUISTORM
          std::cout << "GUIStorm: found new nearest font size " << f->font_size << " (" << f->name << ")" << std::endl;
        #endif // DEBUG_GUISTORM
      }
    } else {
      f = thisfont;
      #ifdef DEBUG_GUISTORM
        std::cout << "GUIStorm: defaulted to font size " << f->font_size << " (" << f->name << ")" << std::endl;
      #endif // DEBUG_GUISTORM
    }
  }
  return f;
}
font *gui::get_font_by_size_or_smaller(float size) {
  /// Attempt to find a font of the specified size, and return the next smallest in size, or nullptr if none smaller found
  #ifdef DEBUG_GUISTORM
    std::cout << "GUIStorm: requested font same or smaller than size " << size << std::endl;
  #endif // DEBUG_GUISTORM
  font *f = nullptr;
  for(auto const &thisfont : fonts) {
    if(thisfont->font_size <= size) {                                           // filter for same size or smaller fonts only
      if(f) {
        if(thisfont->font_size > f->font_size) {                                // compare to get the biggest font size of the filtered fonts
          f = thisfont;
          #ifdef DEBUG_GUISTORM
            std::cout << "GUIStorm: found new nearest font size " << f->font_size << " (" << f->name << ")" << std::endl;
          #endif // DEBUG_GUISTORM
        }
      } else {
        f = thisfont;
        #ifdef DEBUG_GUISTORM
          std::cout << "GUIStorm: defaulted to font size " << f->font_size << " (" << f->name << ")" << std::endl;
        #endif // DEBUG_GUISTORM
      }
    }
  }
  return f;
}
font *gui::get_font_by_size_or_bigger(float size) {
  /// Attempt to find a font of the specified size, and return the next biggest in size, or nullptr if none bigger found
  #ifdef DEBUG_GUISTORM
    std::cout << "GUIStorm: requested font same or bigger than size " << size << std::endl;
  #endif // DEBUG_GUISTORM
  font *f = nullptr;
  for(auto const &thisfont : fonts) {
    if(thisfont->font_size >= size) {                                           // filter for same size or bigger fonts only
      if(f) {
        if(thisfont->font_size < f->font_size) {                                // compare to get the smalleset font size of the filtered fonts
          f = thisfont;
          #ifdef DEBUG_GUISTORM
            std::cout << "GUIStorm: found new nearest font size " << f->font_size << " (" << f->name << ")" << std::endl;
          #endif // DEBUG_GUISTORM
        }
      } else {
        f = thisfont;
        #ifdef DEBUG_GUISTORM
          std::cout << "GUIStorm: defaulted to font size " << f->font_size << " (" << f->name << ")" << std::endl;
        #endif // DEBUG_GUISTORM
      }
    }
  }
  return f;
}
#endif // GUISTORM_NO_TEXT

void gui::set_windowsize(coordtype const &new_windowsize) {
  /// Cache the window size and refresh buffers if it's changed
  #ifdef DEBUG_GUISTORM
    std::cout << "GUIStorm: Window resized to " << new_windowsize << std::endl;
  #endif // DEBUG_GUISTORM
  if(windowsize == new_windowsize) {
    return;
  }
  windowsize = new_windowsize;
  update_layout();                                                              // reposition any window-relative GUI elements
  // update all buffers
  if(glfwGetCurrentContext() != NULL) {                                         // make sure we're in a valid opengl context before we try to refresh
    refresh();
  } else {
    #ifdef DEBUG_GUISTORM
      std::cout << "GUIStorm: Window resize called outside opengl context" << std::endl;
    #endif
  }
}

GLfloat gui::get_dpi() const {
  return dpi;
}
GLfloat gui::get_dpi_scale() const {
  return dpi_scale;
}
void gui::set_dpi(GLfloat newdpi) {
  /// Update dpi and update the cached value of dpi scale
  dpi = std::clamp(newdpi, dpi_min, dpi_max);
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
    cursor->set_position_nodpiscale(cursor_position);                           // don't call the dpi scaler function
    cursor->refresh_position_only();
  }
  update_cursor_pick();
}

void gui::update_cursor_pick() {
  /// Update what the cursor is picking, for instance if windows have changed under the cursor without it having moved
  picked_element = get_picked(cursor_position);                                 // traverse the tree to update the currently picked element
}

void gui::set_mouse_pressed() {
  /// Tell the gui the mouse has been pressed
  mouse_pressed = true;
}
void gui::set_mouse_released() {
  /// Tell the gui the mouse is being released
  mouse_pressed = false;
  #ifndef GUISTORM_NO_TEXT
    if(!picked_element) {
      deselect_input_field();
    }
  #endif // GUISTORM_NO_TEXT
}

coordtype gui::coord_transform(coordtype const &coord) {
  /// Helper to transform screen coordinates into screen space suitable for feeding to the shader without further transformation
  #ifdef GUISTORM_ROUND_NEAREST_OUT
    return coordtype((GUISTORM_ROUND(coord.x) * 2 / windowsize.x) - 1.0f,
                     (GUISTORM_ROUND(coord.y) * 2 / windowsize.y) - 1.0f);
  #else
    return coordtype((coord.x * 2 / windowsize.x) - 1.0f,
                     (coord.y * 2 / windowsize.y) - 1.0f);
  #endif // GUISTORM_ROUND_NEAREST_OUT
}

#ifndef GUISTORM_NO_TEXT
void gui::select_input_field(input_text *new_input_field) {
  /// Select an input field
  if(current_input_field == new_input_field) {                                  // skip if already selected
    return;
  }
  if(current_input_field != nullptr) {                                          // first deselect anything we have selected already
    try {
      function_deselect_input(*current_input_field);                            // call the deselect (unbind) function
    } catch(std::bad_function_call const &e) {
      std::cout << "GUIStorm: ERROR: input deselect for \"" << current_input_field->get_label() << "\" threw exception " << e.what() << std::endl;
    }
    current_input_field->deselected_as_input();                                 // tell the old one it it's been deselected
  }
  current_input_field = new_input_field;
  if(current_input_field != nullptr) {                                          // only call selection function on non-null object
    try {
      function_select_input(*current_input_field);                              // call the select (bind) function
    } catch(std::bad_function_call const &e) {
      std::cout << "GUIStorm: ERROR: input select for \"" << current_input_field->get_label() << "\" threw exception " << e.what() << std::endl;
    }
    current_input_field->selected_as_input();                                   // tell the new one it it's been selected
  }
}

void gui::deselect_input_field() {
  /// Helper wrapper function to deselect any input field
  select_input_field(nullptr);
}
#endif // GUISTORM_NO_TEXT

}
