#include "button.h"

namespace guistorm {

button::button(container *parent,
               colourset const &colours,
               std::string const &label,
               std::function<void()> callback_function,
               font *label_font,
               coordtype const &thissize,
               coordtype const &thisposition)
  : widget(parent, colours, label, label_font, thissize, thisposition),
    function(callback_function) {
  /// Specific constructor
  focusable = true;
}

button::~button() {
  /// Default destructor
}

/*
void button::on_press() {
  /// Process anything that happens when this element is being clicked / held down
  if(parent_gui->mouse_pressed_frames == 0) {           // this is the first frame of a click only
    execute()
  }
}
*/

void button::on_release() {
  /// Process anything that happens when this element was being clicked / held and is released
  #ifdef DEBUG_GUISTORM
    std::cout << "GUIStorm: DEBUG: button " << get_label() << " activating..." << std::endl;
  #endif // DEBUG_GUISTORM
  execute();
  #ifdef DEBUG_GUISTORM
    std::cout << "GUIStorm: DEBUG: button " << get_label() << " finished" << std::endl;
  #endif // DEBUG_GUISTORM
}

void button::execute() {
  /// Wrapper to call this button's function and handle any errors
  try {
    function();
  } catch(const std::bad_function_call &e) {
    std::cout << "GUIStorm: ERROR: button \"" << get_label() << "\" threw exception " << e.what() << std::endl;
  }
}

}
