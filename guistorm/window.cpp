#include "window.h"
#include "gui.h"

namespace guistorm {

window::window(container *parent,
               colourset const &colours,
               std::string const &label,
               font *label_font,
               coordtype const &thissize,
               coordtype const &thisposition)
  : base(parent, colours, label, label_font, thissize, thisposition) {
  /// Specific constructor
}

window::~window() {
  /// Default destructor
}

void window::add_to_gui(base *element) {
  /// Add the specified element to the top parent gui
  if(parent_gui) {
    parent_gui->add_to_gui(element);
  }
}

base *window::get_picked(coordtype const &cursor_position) {
  /// Return true if the mouse is over this element
  if(!base::get_picked(cursor_position)) {
    return nullptr;                   // we're picking outside this window
  }
  // also run the check recursively on each child object
  base *picked_element(container::get_picked(cursor_position));
  if(picked_element) {
    return picked_element;            // we're picking a child element in this window
  } else {
    return this;                      // we're picking the window itself
  }
}

coordtype const window::get_absolute_position() const {
  /// Return the absolute screen coords of the origin of this element
  return base::get_absolute_position();    // this is needed since container class has its own function to retrieve absolute position that we must override
}

void window::stretch_vertical(GLfloat margin) {
  /// Expand or shrink this window to include all its elements if fitted in layout_vertical()
  stretch_vertical(elements.begin(), elements.end(), margin);                       // wrapper: adding element range
}
void window::stretch_vertical(std::vector<base*>::const_iterator first,
                              std::vector<base*>::const_iterator last,
                              GLfloat margin) {
  /// Expand or shrink this window to include the selected elements if fitted in layout_vertical()
  coordtype newsize(0.0, margin);
  for(auto it = first; it != last; ++it) {
    newsize.y += (*it)->get_size().y + margin;                                      // stack the verticals
    newsize.x = std::max(newsize.x, (*it)->get_size().x);                           // rubber-band to widest horizontal
  }
  newsize.x += margin * 2.0;
  set_size(newsize);
}
void window::stretch_horizontal(GLfloat margin) {
  /// Expand or shrink this window to include all its elements if fitted in layout_horizontal()
  stretch_horizontal(elements.begin(), elements.end(), margin);                     // wrapper: adding element range
}
void window::stretch_horizontal(std::vector<base*>::const_iterator first,
                                std::vector<base*>::const_iterator last,
                                GLfloat margin) {
  /// Expand or shrink this window to include the selected elements if fitted in layout_horizontal()
  coordtype newsize(margin, 0.0);
  for(auto it = first; it != last; ++it) {
    newsize.x += (*it)->get_size().x + margin;                                      // stack the horizontals
    newsize.y = std::max(newsize.y, (*it)->get_size().y);                           // rubber-band to tallest vertical
  }
  newsize.y += margin * 2.0;
  set_size(newsize);
}

void window::layout_vertical(GLfloat margin) {
  /// Distribute all contained elements to fill the whole available space vertically with a set margin
  layout_vertical(elements.begin(), elements.end(), margin);                        // wrapper: adding element range
}
void window::layout_vertical(coordtype const &bottomleft,
                             coordtype const &topright) {
  /// Distribute all contained elements to fill the selected space vetically
  if(topright.x == 0.0 && topright.y == 0.0) {
    layout_vertical(elements.begin(), elements.end(), bottomleft, get_size());      // wrapper: adding element range, default size
  } else {
    layout_vertical(elements.begin(), elements.end(), bottomleft, topright);        // wrapper: adding element range
  }
}
void window::layout_vertical(std::vector<base*>::const_iterator first,
                             std::vector<base*>::const_iterator last,
                             GLfloat margin) {
  /// Distribute the selected contained elements to fill the whole available space vertically with a set margin
  layout_vertical(first, last, coordtype(margin, margin), get_size() - margin);     // wrapper: convert margin to coords
}
void window::layout_vertical(std::vector<base*>::const_iterator first,
                             std::vector<base*>::const_iterator last,
                             coordtype const &bottomleft,
                             coordtype const &topright) {
  /// Distribute the selected contained elements to fill the selected space vertically
  GLfloat totalheight = 0.0;                                                        // calculate total heights
  for(auto it = first; it != last; ++it) {
    totalheight += (*it)->get_size().y;
  }
  GLfloat const range = topright.y - bottomleft.y;
  GLfloat const margin = (range - totalheight) / (std::distance(first, last) - 1);  // take 1 to allow marginless fitting
  GLfloat pen = topright.y;
  for(auto it = first; it != last; ++it) {                                          // distribute evenly within the space
    pen -= (*it)->get_size().y;
    (*it)->set_position((((topright.x - bottomleft.x) - (*it)->get_size().x) / 2.0) + bottomleft.x, pen);  // centre each element
    pen -= margin;
  }
}

void window::layout_horizontal(GLfloat margin) {
  /// Distribute all contained elements to fill the whole available space horizontally with a set margin
  layout_horizontal(elements.begin(), elements.end(), margin);                      // wrapper: adding element range
}
void window::layout_horizontal(coordtype const &bottomleft,
                               coordtype const &topright) {
  /// Distribute all contained elements to fill the selected space horizontally
  if(topright.x == 0.0 && topright.y == 0.0) {
    layout_horizontal(elements.begin(), elements.end(), bottomleft, get_size());    // wrapper: adding element range, default size
  } else {
    layout_horizontal(elements.begin(), elements.end(), bottomleft, topright);      // wrapper: adding element range
  }
}
void window::layout_horizontal(std::vector<base*>::const_iterator first,
                               std::vector<base*>::const_iterator last,
                               GLfloat margin) {
  /// Distribute the selected contained elements to fill the whole available space horizontally with a set margin
  layout_horizontal(first, last, coordtype(margin, margin), get_size() - margin);   // wrapper: convert margin to coords
}
void window::layout_horizontal(std::vector<base*>::const_iterator first,
                               std::vector<base*>::const_iterator last,
                               coordtype const &bottomleft,
                               coordtype const &topright) {
  /// Distribute the selected contained elements to fill the selected space horizontally
  GLfloat totalwidth = 0.0;                                                         // calculate total heights
  for(auto it = first; it != last; ++it) {
    totalwidth += (*it)->get_size().x;
  }
  GLfloat const range = topright.x - bottomleft.x;
  GLfloat const margin = (range - totalwidth) / (std::distance(first, last) - 1);   // take 1 to allow marginless fitting
  GLfloat pen = bottomleft.x;
  for(auto it = first; it != last; ++it) {                                          // distribute evenly within the space
    (*it)->set_position(pen, (((topright.y - bottomleft.y) - (*it)->get_size().y) / 2.0) + bottomleft.y); // centre each element
    pen += (*it)->get_size().x + margin;
  }
}

void window::destroy_buffer() {
  base::destroy_buffer();
  container::destroy_buffer();
}

void window::update_layout() {
  /// Reposition this object and its contents in accordance with any layout rules given to it
  base::update_layout();
  container::update_layout();
}

void window::refresh() {
  /// Refresh this object's visual state and all its children
  base::refresh();
  container::refresh();
}

void window::render() {
  /// Draw this element and all child elements
  if(!visible) {
    return;
  }
  base::render();
  container::render();
}

}
