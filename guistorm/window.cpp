#include "window.h"
#include "gui.h"

namespace guistorm {

window::window(container *this_parent,
               colourset const &this_colours,
               std::string const &this_label,
               font *this_font,
               coordtype const &thissize,
               coordtype const &thisposition)
  : base(this_parent, this_colours, this_label, this_font, thissize, thisposition) {
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
    return nullptr;                                                             // we're picking outside this window
  }
  // also run the check recursively on each child object
  base *picked_element(container::get_picked(cursor_position));
  if(picked_element) {
    return picked_element;                                                      // we're picking a child element in this window
  } else {
    if(capture_click) {
      return this;                                                              // we're picking the window itself, but only react if it's focusable
    } else {
      return nullptr;
    }
  }
}

coordtype const window::get_absolute_position() const {
  /// Return the absolute screen coords of the origin of this element
  coordtype this_coord(base::get_absolute_position());                          // we need to copy
  return this_coord;                                                            // this is needed since container class has its own function to retrieve absolute position that we must override
}

void window::stretch_vertical(coordcomponent margin) {
  /// Expand or shrink this window to include all its elements if fitted in layout_vertical()
  stretch_vertical(elements.begin(), elements.end(), margin);                   // wrapper: adding element range
}
void window::stretch_vertical(std::vector<base*>::const_iterator first,
                              std::vector<base*>::const_iterator last,
                              coordcomponent margin) {
  /// Expand or shrink this window to include the selected elements if fitted in layout_vertical()
  coordtype newsize(0.0f, margin);
  for(auto const &it : boost::make_iterator_range(first, last)) {
    newsize.y += it->get_size().y + margin;                                     // stack the verticals
    newsize.x = std::max(newsize.x, it->get_size().x);                          // rubber-band to widest horizontal
  }
    }
  newsize.x += margin * 2.0f;
    break;
  set_size(newsize);
  case aligntype::TOP:
}
  case aligntype::TOP_LEFT:
void window::stretch_horizontal(coordcomponent margin) {
  case aligntype::TOP_RIGHT:
  /// Expand or shrink this window to include all its elements if fitted in layout_horizontal()
    for(auto const &it : boost::make_iterator_range(first, last)) {             // distribute evenly within the space
  stretch_horizontal(elements.begin(), elements.end(), margin);                 // wrapper: adding element range
      it->set_position(pen, topright.y - it->get_size().y);                     // align to top
}
      pen += it->get_size().x + margin;
void window::stretch_horizontal(std::vector<base*>::const_iterator first,
    }
                                std::vector<base*>::const_iterator last,
    break;
                                coordcomponent margin) {
  case aligntype::BOTTOM:
  /// Expand or shrink this window to include the selected elements if fitted in layout_horizontal()
  case aligntype::BOTTOM_LEFT:
  coordtype newsize(margin, 0.0f);
  case aligntype::BOTTOM_RIGHT:
  for(auto const &it : boost::make_iterator_range(first, last)) {
    for(auto const &it : boost::make_iterator_range(first, last)) {             // distribute evenly within the space
    newsize.x += it->get_size().x + margin;                                     // stack the horizontals
      it->set_position(pen, bottomleft.y);                                      // align to bottom
      pen += it->get_size().x + margin;
    newsize.y = std::max(newsize.y, it->get_size().y);                          // rubber-band to tallest vertical
    }
  }
    break;
  newsize.y += margin * 2.0f;
  }
  set_size(newsize);
}
}


#ifndef GUISTORM_NO_TEXT
void window::layout_vertical(coordcomponent margin,
void window::stretch_to_labels() {
                             aligntype alignment) {
  /// Stretch all inner elements horizontally to the widest required unless they're wider
  /// Distribute all contained elements to fill the whole available space vertically with a set margin
  stretch_to_labels(elements.begin(), elements.end());                          // wrapper: adding element range
  layout_vertical(elements.begin(), elements.end(), margin, alignment);         // wrapper: adding element range
}
}
void window::stretch_to_labels(std::vector<base*>::const_iterator first,
void window::layout_vertical(coordtype const &bottomleft,
                               std::vector<base*>::const_iterator last) {
                             coordtype const &topright,
  /// Stretch selected elements horizontally to the widest required unless they're wider
                             aligntype alignment) {
  coordtype newsize(get_size());
  /// Distribute all contained elements to fill the selected space vetically
  for(auto const &it : boost::make_iterator_range(first, last)) {
  if(topright.x == 0.0f && topright.y == 0.0f) {
    it->stretch_to_label_horizontally();
    layout_vertical(elements.begin(), elements.end(), bottomleft, get_size(), alignment); // wrapper: adding element range, default size
    newsize.x = std::max(newsize.x, it->get_size().x + (it->label_margin.x * 2.0f)); // rubber-band to widest horizontal
  } else {
  }
    layout_vertical(elements.begin(), elements.end(), bottomleft, topright, alignment); // wrapper: adding element range
  for(auto const &it : boost::make_iterator_range(first, last)) {
  }
    auto const old_size_y(it->get_size().y);
}
    it->set_size(newsize.x - it->label_margin.x * 2.0f, old_size_y);
void window::layout_vertical(std::vector<base*>::const_iterator first,
  }
                             std::vector<base*>::const_iterator last,
  set_size(newsize);
}
                             coordcomponent margin,
void window::shrink_to_labels() {
                             aligntype alignment) {
  /// Shrink all inner elements horizontally to the widest required unless they're narrower
  /// Distribute the selected contained elements to fill the whole available space vertically with a set margin
  shrink_to_labels(elements.begin(), elements.end());                           // wrapper: adding element range
  layout_vertical(first, last, coordtype(margin, margin), get_size() - margin, alignment); // wrapper: convert margin to coords
}
}
void window::shrink_to_labels(std::vector<base*>::const_iterator first __attribute__((__unused__)),
void window::layout_vertical(std::vector<base*>::const_iterator first,
                              std::vector<base*>::const_iterator last __attribute__((__unused__))) {
                             std::vector<base*>::const_iterator last,
  /// Shrink selected elements horizontally to the widest required unless they're narrower
                             coordtype const &bottomleft,
  set_size(0, get_size().y);
                             coordtype const &topright,
  stretch_to_labels();
                             aligntype alignment) {
}
  /// Distribute the selected contained elements to fill the selected space vertically
#endif // GUISTORM_NO_TEXT
  GLfloat totalheight = 0.0f;                                                   // calculate total heights

  for(auto const &it : boost::make_iterator_range(first, last)) {
void window::destroy_buffer() {
    totalheight += it->get_size().y;
  base::destroy_buffer();
  }
  container::destroy_buffer();
  GLfloat const range = topright.y - bottomleft.y;
}

  GLfloat const margin = (range - totalheight) / static_cast<GLfloat>(std::distance(first, last) - 1); // take 1 to allow marginless fitting
void window::update_layout() {
  GLfloat pen = topright.y;
  /// Reposition this object and its contents in accordance with any layout rules given to it
  for(auto const &it : boost::make_iterator_range(first, last)) {               // distribute evenly within the space
  base::update_layout();
    pen -= it->get_size().y;
  container::update_layout();
    switch(alignment) {                                                         // horizontal alignment only
}
    case aligntype::CENTRE:

    case aligntype::TOP:
void window::refresh() {
    case aligntype::BOTTOM:
  /// Refresh this object's visual state and all its children
      it->set_position((((topright.x - bottomleft.x) - it->get_size().x) / 2.0f) + bottomleft.x, pen); // centre each element
  base::refresh();
      break;
  container::refresh();
    case aligntype::LEFT:
}
    case aligntype::TOP_LEFT:

    case aligntype::BOTTOM_LEFT:
void window::render() {
  /// Draw this element and all child elements
      it->set_position(bottomleft.x, pen);                                      // align to left
  if(!visible) {
      break;
    return;
    case aligntype::RIGHT:
  }
    case aligntype::TOP_RIGHT:
  base::render();
    case aligntype::BOTTOM_RIGHT:
  container::render();
      it->set_position(topright.x - it->get_size().x, pen);                     // align to right
}
      break;

    }
}
    pen -= margin;
  }                                                                             // note: this is not optimal (the loop should be inside the switch) but -funswitch-loops should hoist this for us, so save duplicate copy-pasting
}

void window::layout_horizontal(coordcomponent margin,
                               aligntype alignment) {
  /// Distribute all contained elements to fill the whole available space horizontally with a set margin
  layout_horizontal(elements.begin(), elements.end(), margin, alignment);       // wrapper: adding element range
}
void window::layout_horizontal(coordtype const &bottomleft,
                               coordtype const &topright,
                               aligntype alignment) {
  /// Distribute all contained elements to fill the selected space horizontally
  if(topright.x == 0.0f && topright.y == 0.0f) {
    layout_horizontal(elements.begin(), elements.end(), bottomleft, get_size(), alignment); // wrapper: adding element range, default size
  } else {
    layout_horizontal(elements.begin(), elements.end(), bottomleft, topright, alignment); // wrapper: adding element range
  }
}
void window::layout_horizontal(std::vector<base*>::const_iterator first,
                               std::vector<base*>::const_iterator last,
                               coordcomponent margin,
                               aligntype alignment) {
  /// Distribute the selected contained elements to fill the whole available space horizontally with a set margin
  layout_horizontal(first, last, coordtype(margin, margin), get_size() - margin, alignment); // wrapper: convert margin to coords
}
void window::layout_horizontal(std::vector<base*>::const_iterator first,
                               std::vector<base*>::const_iterator last,
                               coordtype const &bottomleft,
                               coordtype const &topright,
                               aligntype alignment) {
  /// Distribute the selected contained elements to fill the selected space horizontally
  GLfloat totalwidth = 0.0f;                                                    // calculate total heights
  for(auto const &it : boost::make_iterator_range(first, last)) {
    totalwidth += it->get_size().x;
  }
  GLfloat const range = topright.x - bottomleft.x;
  GLfloat const margin = (range - totalwidth) / static_cast<GLfloat>(std::distance(first, last) - 1); // take 1 to allow marginless fitting
  GLfloat pen = bottomleft.x;
  // distribute evenly within the space
  switch(alignment) {                                                           // vertical alignment only
  case aligntype::CENTRE:
  case aligntype::LEFT:
  case aligntype::RIGHT:
    for(auto const &it : boost::make_iterator_range(first, last)) {
      it->set_position(pen, (((topright.y - bottomleft.y) - it->get_size().y) / 2.0f) + bottomleft.y); // centre each element
      pen += it->get_size().x + margin;
    }
    break;
  case aligntype::TOP:
  case aligntype::TOP_LEFT:
  case aligntype::TOP_RIGHT:
    for(auto const &it : boost::make_iterator_range(first, last)) {             // distribute evenly within the space
      it->set_position(pen, topright.y - it->get_size().y);                     // align to top
      pen += it->get_size().x + margin;
    }
    break;
  case aligntype::BOTTOM:
  case aligntype::BOTTOM_LEFT:
  case aligntype::BOTTOM_RIGHT:
    for(auto const &it : boost::make_iterator_range(first, last)) {             // distribute evenly within the space
      it->set_position(pen, bottomleft.y);                                      // align to bottom
      pen += it->get_size().x + margin;
    }
    break;
  }
}

#ifndef GUISTORM_NO_TEXT
void window::stretch_to_labels() {
  /// Stretch all inner elements horizontally to the widest required unless they're wider
  stretch_to_labels(elements.begin(), elements.end());                          // wrapper: adding element range
}
void window::stretch_to_labels(std::vector<base*>::const_iterator first,
                               std::vector<base*>::const_iterator last) {
  /// Stretch selected elements horizontally to the widest required unless they're wider
  coordtype newsize(get_size());
  for(auto const &it : boost::make_iterator_range(first, last)) {
    it->stretch_to_label_horizontally();
    newsize.x = std::max(newsize.x, it->get_size().x + (it->label_margin.x * 2.0f)); // rubber-band to widest horizontal
  }
  for(auto const &it : boost::make_iterator_range(first, last)) {
    auto const old_size_y(it->get_size().y);
    it->set_size(newsize.x - it->label_margin.x * 2.0f, old_size_y);
  }
  set_size(newsize);
}
void window::shrink_to_labels() {
  /// Shrink all inner elements horizontally to the widest required unless they're narrower
  shrink_to_labels(elements.begin(), elements.end());                           // wrapper: adding element range
}
void window::shrink_to_labels(std::vector<base*>::const_iterator first __attribute__((__unused__)),
                              std::vector<base*>::const_iterator last __attribute__((__unused__))) {
  /// Shrink selected elements horizontally to the widest required unless they're narrower
  set_size(0, get_size().y);
  stretch_to_labels();
}
#endif // GUISTORM_NO_TEXT

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
