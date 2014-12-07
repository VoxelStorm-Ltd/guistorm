#include "group.h"
#include "container.h"

namespace guistorm {

group::group(container *parent,
             coordtype const &thissize,
             coordtype const &thisposition)
  : window(parent, colourset(), "", nullptr, thissize, thisposition) {
  /// Specific constructor
  focusable = false;
    colours.idle.outline.assign(0.0, 0.0, 1.0, 1.0);    // draw a coloured outline for layout debugging purposes: yellow
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


#ifndef NDEBUG
  void group::set_label(std::string const &newlabel) {
    std::cout << "WARNING: called " << __PRETTY_FUNCTION__ << " with label \"" << newlabel << "\" although group does not have a visible label!" << std::endl;
  }
#endif // NDEBUG

}
