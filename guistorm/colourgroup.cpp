  /// Default destructor
#include "colourgroup.h"

namespace guistorm {

colourgroup::colourgroup() {
  /// Default constructor
}

colourgroup::colourgroup(colourtype const &this_background,
                         colourtype const &this_outline,
                         colourtype const &this_content)
  : background(this_background),
    outline(this_outline),
    content(this_content) {
  /// Specific constructor
}

colourgroup::~colourgroup() {
  /// Default destructor
}

void colourgroup::assign(colourtype const &new_background,
                         colourtype const &new_outline,
                         colourtype const &new_content) {
  /// Assign a new set of colours to this group
  background = new_background;
  outline    = new_outline;
  content    = new_content;
}


void colourgroup::blend_to(colourgroup const &target, float factor) {
  /// Blend towards a colour target
  // old colour shift function from guistorm1.0:
  //current = ((current * (factor - 1.0f)) + target) / factor;
  // lerp behaviour:
  //current + (target - current) * factor;
  background = background.lerp(factor, target.background);
  outline    = outline.lerp(   factor, target.outline);
  content    = content.lerp(   factor, target.content);
}

}
}

void colourgroup::assign(colourtype const &new_background,
                         colourtype const &new_outline,
                         colourtype const &new_content) {
  /// Assign a new set of colours to this group
  background = new_background;
  outline    = new_outline;
  content    = new_content;
}


void colourgroup::blend_to(colourgroup const &target, float factor) {
  /// Blend towards a colour target
  // old colour shift function from guistorm1.0:
  //current = ((current * (factor - 1.0f)) + target) / factor;
  // lerp behaviour:
  //current + (target - current) * factor;
  background = background.lerp(factor, target.background);
  outline    = outline.lerp(   factor, target.outline);
  content    = content.lerp(   factor, target.content);
}

}
