#ifndef GUISTORM_NO_TEXT

#include "label.h"

namespace guistorm {

label::label(container *newparent,
             colourset const &newcolours,
             std::string const &newlabel,
             font *newlabel_font,
             coordtype const &thissize,
             coordtype const &thisposition)
  : widget(newparent, newcolours, newlabel, newlabel_font, thissize, thisposition) {
  /// Specific constructor
  focusable = false;
  #ifdef DEBUG_GUISTORM
    colours.idle.outline.assign(1.0, 1.0, 0.0, 1.0);                            // draw a coloured outline for layout debugging purposes: yellow
  #endif // DEBUG_GUISTORM
}

label::~label() {
  /// Default destructor
}

base *label::get_picked(coordtype const &cursor_position __attribute__((__unused__))) {
  /// Labels are never clickable
  return nullptr;
}

#ifndef DEBUG_GUISTORM
  void label::init_buffer() {
    /// Generate the buffers for this object
    // skip initialising unused outline buffers
    glGenBuffers(1, &vbo_label);
    glGenBuffers(1, &ibo_label);
  }
  void label::destroy_buffer() {
    /// Clean up the buffers in preparation for exit or context switch
    // skip destroying unused outline buffers
    glDeleteBuffers(1, &vbo_label);
    glDeleteBuffers(1, &ibo_label);
    //if(label_font) {
    //  label_font->unload();
    //}
    vbo_label = 0;
    ibo_label = 0;
    numverts_label = 0;
    initialised = false;
  }

  void label::setup_buffer() {
    /// Create or update the buffer for this element
    if(__builtin_expect(vbo_label == 0, 0)) {                                   // if the buffer hasn't been generated yet (unlikely)
      init_buffer();
    }
    numverts = 0;                                                               // mark the outline and fill buffer as unused

    setup_label();                                                              // set up the label buffer

    initialised = true;
  }
#endif // DEBUG_GUISTORM

}

#endif // GUISTORM_NO_TEXT
