#include "colourset.h"

namespace guistorm {

colourset::colourset() {
  /// Default constructor
}

colourset::colourset(colourgroup const &colour_current,
                     colourgroup const &colour_idle,
                     colourgroup const &colour_hover,
                     colourgroup const &colour_focus,
                     colourgroup const &colour_active)
  : current(colour_current),
    idle(colour_idle),
    hover(colour_hover),
    focus(colour_focus),
    active(colour_active) {
  /// Specific constructor
}

colourset::~colourset() {
  /// Default destructor
}

void colourset::assign(colourgroup const &new_current,
                       colourgroup const &new_idle,
                       colourgroup const &new_hover,
                       colourgroup const &new_focus,
                       colourgroup const &new_active) {
  /// Assign a new set of colourgroups to this set
  current = new_current;
  idle    = new_idle;
  hover   = new_hover;
  focus   = new_focus;
  active  = new_active;
}

void colourset::blend_to(colourgroup const &target, float factor) {
  /// Blend towards a colour target
  current.blend_to(target, factor);
}

void colourset::blend_to_idle(float factor) {
  /// Shortcuts to blend towards existing colours
  blend_to(idle, factor);
}
void colourset::blend_to_hover(float factor) {
  blend_to(hover, factor);
}
void colourset::blend_to_focus(float factor) {
  blend_to(focus, factor);
}
void colourset::blend_to_active(float factor) {
  blend_to(active, factor);
}

}
