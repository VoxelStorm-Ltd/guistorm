#pragma once

#include "colourgroup.h"

namespace guistorm {

class colourset {
public:
  // a complete set of colours for this widget
  colourgroup current;                                                          // current colour of this element

  colourgroup idle;                                                             // default colours at idle
  colourgroup hover;                                                            // mouseover colours
  colourgroup focus;                                                            // focus colours (when an element is currently selected for input)
  colourgroup active;                                                           // active colours (while mouse_pressed / when text is being entered)

  colourset();
  colourset(colourgroup const &current,
            colourgroup const &idle,
            colourgroup const &hover,
            colourgroup const &focus,
            colourgroup const &active);
  ~colourset();

  void assign(colourgroup const &new_current,
              colourgroup const &new_idle,
              colourgroup const &new_hover,
              colourgroup const &new_focus,
              colourgroup const &new_active);

  void blend_to(colourgroup const &target, float factor);
  void blend_to_idle(  float factor);
  void blend_to_hover( float factor);
  void blend_to_focus( float factor);
  void blend_to_active(float factor);
};

}
