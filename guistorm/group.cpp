#include "group.h"
#include <iostream>

namespace guistorm {

group::group(container *newparent,
             coordtype const &thissize,
             coordtype const &thisposition)
  : window(newparent, colourset(), "", nullptr, thissize, thisposition) {
  /// Specific constructor
  focusable = false;
  #ifdef DEBUG_GUISTORM
    colours.idle.outline.assign(0.0, 0.0, 1.0, 1.0);    // draw a coloured outline for layout debugging purposes: yellow
  #endif // DEBUG_GUISTORM
}

group::~group() {
  /// Default destructor
}

base *group::get_picked(coordtype const &cursor_position) {
  /// Return true if the mouse is over this element
  if(!visible) {
    return nullptr;
  }
  return container::get_picked(cursor_position);          // bypass the window checking and don't return this if we miss any child objects
}

#ifndef DEBUG_GUISTORM
  void group::init_buffer() {
    /// noop since nothing is drawn
  }
  void group::destroy_buffer() {
    /// noop since nothing is drawn
    initialised = false;
    container::destroy_buffer();
  }

  void group::setup_buffer() {
    /// Create or update the buffer for this element
    numverts = 0;                                 // mark the outline and fill buffer as unused
    // skip initialising any visible content
    initialised = true;
  }

  void group::render() {
    /// Render the relevant children of this object, but skip rendering itself
    if(!visible) {
      return;
    }
    container::render();
  }
#endif // DEBUG_GUISTORM

#ifndef NDEBUG
  void group::set_label(std::string const &newlabel) {
    std::cout << "WARNING: called " << __PRETTY_FUNCTION__ << " with label \"" << newlabel << "\" although group does not have a visible label!" << std::endl;
  }
#endif // NDEBUG

}
