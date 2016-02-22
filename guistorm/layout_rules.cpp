//#include "layout_rules.h"
#include <iostream>
#include "gui.h"
#include "base.h"

namespace guistorm {
namespace layout {

void centre_horizontally(layout::targettype target) {
  /// Position this element centered horizontally to its parent
  base *parent_base = dynamic_cast<base*>(target.parent);
  if(parent_base) {
    target.set_position_nodpiscale((parent_base->get_size_nodpiscale().x - target.get_size_nodpiscale().x) / 2.0f, target.get_position_nodpiscale().y);
  } else {
    #ifndef NDEBUG
      if(!target.parent_gui) {
        std::cout << "GUIStorm: Layout: ERROR: " << __PRETTY_FUNCTION__ << " asked to perform layout on object belonging to no gui" << std::endl;
        return;
      }
    #endif // NDEBUG
    target.set_position_nodpiscale((target.parent_gui->windowsize.x - target.get_size_nodpiscale().x) / 2.0f, target.get_position_nodpiscale().y);
  }
}

void centre_vertically(layout::targettype target) {
  /// Position this element centered vertically to its parent
  base *parent_base = dynamic_cast<base*>(target.parent);
  if(parent_base) {
    target.set_position_nodpiscale(target.get_position_nodpiscale().x, (parent_base->get_size_nodpiscale().y - target.get_size_nodpiscale().y) / 2.0f);
  } else {
    #ifndef NDEBUG
      if(!target.parent_gui) {
        std::cout << "GUIStorm: Layout: ERROR: " << __PRETTY_FUNCTION__ << " asked to perform layout on object belonging to no gui" << std::endl;
        return;
      }
    #endif // NDEBUG
    target.set_position_nodpiscale(target.get_position_nodpiscale().x, (target.parent_gui->windowsize.y - target.get_size_nodpiscale().y) / 2.0f);
  }
}
void centre(layout::targettype target) {
  /// Position this element centered on both axes to its parent
  base *parent_base = dynamic_cast<base*>(target.parent);
  if(parent_base) {
    target.set_position_nodpiscale((parent_base->get_size_nodpiscale() - target.get_size_nodpiscale()) / 2.0f);
  } else {
    #ifndef NDEBUG
      if(!target.parent_gui) {
        std::cout << "GUIStorm: Layout: ERROR: " << __PRETTY_FUNCTION__ << " asked to perform layout on object belonging to no gui" << std::endl;
        return;
      }
    #endif // NDEBUG
    target.set_position_nodpiscale((target.parent_gui->windowsize - target.get_size_nodpiscale()) / 2.0f);
  }
}
void offset_left(layout::targettype target, GLfloat distance) {
  /// Position this element in from the left edge of the parent by the specified optional distance
  target.set_position_nodpiscale(distance, target.get_position_nodpiscale().y);
}
void offset_bottom(layout::targettype target, GLfloat distance) {
  /// Position this element in from the bottom edge of the parent by the specified optional distance
  target.set_position_nodpiscale(target.get_position_nodpiscale().x, distance);
}
void offset_right(layout::targettype target, GLfloat distance) {
  /// Position this element in from the right edge of the parent by the specified optional distance
  base *parent_base = dynamic_cast<base*>(target.parent);
  if(parent_base) {
    target.set_position_nodpiscale((parent_base->get_size_nodpiscale().x - target.get_size_nodpiscale().x) - distance, target.get_position_nodpiscale().y);
  } else {
    #ifndef NDEBUG
      if(!target.parent_gui) {
        std::cout << "GUIStorm: Layout: ERROR: " << __PRETTY_FUNCTION__ << " asked to perform layout on object belonging to no gui" << std::endl;
        return;
      }
    #endif // NDEBUG
    target.set_position_nodpiscale((target.parent_gui->windowsize.x - target.get_size_nodpiscale().x) - distance, target.get_position_nodpiscale().y);
  }
}
void offset_top(layout::targettype target, GLfloat distance) {
  /// Position this element in from the bottom top of the parent by the specified optional distance
  base *parent_base = dynamic_cast<base*>(target.parent);
  if(parent_base) {
    target.set_position_nodpiscale(target.get_position_nodpiscale().x, (parent_base->get_size_nodpiscale().y - target.get_size_nodpiscale().y) - distance);
  } else {
    #ifndef NDEBUG
      if(!target.parent_gui) {
        std::cout << "GUIStorm: Layout: ERROR: " << __PRETTY_FUNCTION__ << " asked to perform layout on object belonging to no gui" << std::endl;
        return;
      }
    #endif // NDEBUG
    target.set_position_nodpiscale(target.get_position_nodpiscale().x, (target.parent_gui->windowsize.y - target.get_size_nodpiscale().y) - distance);
  }
}

void fit_horizontally(targettype target, GLfloat margin) {
  /// Position and scale this element to take up the full width of its parent minus the specified optional margin
  base *parent_base = dynamic_cast<base*>(target.parent);
  target.set_position_nodpiscale(margin, target.get_position_nodpiscale().y);
  if(parent_base) {
    target.set_size_nodpiscale(parent_base->get_size_nodpiscale().x - (margin * 2.0f), target.get_size_nodpiscale().y);
  } else {
    #ifndef NDEBUG
      if(!target.parent_gui) {
        std::cout << "GUIStorm: Layout: ERROR: " << __PRETTY_FUNCTION__ << " asked to perform layout on object belonging to no gui" << std::endl;
        return;
      }
    #endif // NDEBUG
    target.set_size_nodpiscale(target.parent_gui->windowsize.x - (margin * 2.0f), target.get_size_nodpiscale().y);
  }
}
void fit_vertically(targettype target, GLfloat margin) {
  /// Position and scale this element to take up the full height of its parent minus the specified optional margin
  base *parent_base = dynamic_cast<base*>(target.parent);
  target.set_position_nodpiscale(target.get_position_nodpiscale().x, margin);
  if(parent_base) {
    target.set_size_nodpiscale(target.get_size_nodpiscale().x, parent_base->get_size_nodpiscale().y - (margin * 2.0f));
  } else {
    #ifndef NDEBUG
      if(!target.parent_gui) {
        std::cout << "GUIStorm: Layout: ERROR: " << __PRETTY_FUNCTION__ << " asked to perform layout on object belonging to no gui" << std::endl;
        return;
      }
    #endif // NDEBUG
    target.set_size_nodpiscale(target.get_size_nodpiscale().x, target.parent_gui->windowsize.y - (margin * 2.0f));
  }
}
void fit(targettype target, GLfloat margin) {
  /// Position and scale this element to take up the full size of its parent minus the specified optional margin
  base *parent_base = dynamic_cast<base*>(target.parent);
  target.set_position_nodpiscale(margin, margin);
  if(parent_base) {
    target.set_size_nodpiscale(parent_base->get_size_nodpiscale() - (margin * 2.0f));
  } else {
    #ifndef NDEBUG
      if(!target.parent_gui) {
        std::cout << "GUIStorm: Layout: ERROR: " << __PRETTY_FUNCTION__ << " asked to perform layout on object belonging to no gui" << std::endl;
        return;
      }
    #endif // NDEBUG
    target.set_size_nodpiscale(target.parent_gui->windowsize - (margin * 2.0f));
  }
}
void move(targettype target, coordtype offset) {
  /// Offset the element's position relative to what it previously was
  target.set_position_nodpiscale(target.get_position_nodpiscale() + offset);
}
#ifndef NDEBUG
  void test_null(targettype target __attribute__((__unused__))) {
    /// Test action that reports and does nothing else
    std::cout << "GUIStorm: Layout: DEBUG: " << __PRETTY_FUNCTION__ << " called" << std::endl;
  }
  void test_text(targettype target __attribute__((__unused__)), std::string text) {
    /// Test action that echoes a string parameter
    std::cout << "GUIStorm: Layout: DEBUG: " << __PRETTY_FUNCTION__ << " called with string: \"" << text << "\"" << std::endl;
  }
#endif

}

}
