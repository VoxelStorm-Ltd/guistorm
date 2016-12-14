#ifndef GUISTORM_LAYOUT_RULES_H_INCLUDED
#define GUISTORM_LAYOUT_RULES_H_INCLUDED

#include <functional>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "types.h"

namespace guistorm {

class base;

namespace layout {
/**
  * Flexible function-driven system of layout rules.  Every base object can
  * be given multiple layout rules and they will be applied in the specific
  * order in which they are stored in base::layout_rules.  The system can be
  * extended by simply creating new rules following the rule typedef.
  *
  * Example usage:
  *   myobject->layout_rules.emplace_back(guistorm::layout::centre_horizontally);
  *   myobject->layout_rules.emplace_back([](guistorm::layout::targettype target){guistorm::layout::centre_horizontally(target);});
  *
  **/

using targettype = guistorm::base&;
using rule = std::function<void()>;

void centre_horizontally(targettype target);
void centre_vertically(  targettype target);
void centre(             targettype target);
void offset_left(        targettype target, coordcomponent distance = 0.0f);
void offset_bottom(      targettype target, coordcomponent distance = 0.0f);
void offset_right(       targettype target, coordcomponent distance = 0.0f);
void offset_top(         targettype target, coordcomponent distance = 0.0f);
void fit_horizontally(   targettype target, coordcomponent margin   = 0.0f);
void fit_vertically(     targettype target, coordcomponent margin   = 0.0f);
void fit(                targettype target, coordcomponent margin   = 0.0f);
void move(               targettype target, coordcomponent offset_x, coordcomponent offset_y);
#ifndef NDEBUG
  void test_null(targettype target);
  void test_text(targettype target, std::string text);
#endif
}

}

#endif // GUISTORM_LAYOUT_RULES_H_INCLUDED
