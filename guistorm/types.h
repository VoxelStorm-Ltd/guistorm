#ifndef GUISTORM_TYPES_H_INCLUDED
#define GUISTORM_TYPES_H_INCLUDED

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "vectorstorm/vector/vector2.h"
#include "vectorstorm/vector/vector4.h"

namespace guistorm {

// types
#ifdef GUISTORM_ROUND_STOREINT
  using coordcomponent = GLint;
#else
  using coordcomponent = GLfloat;
#endif // GUISTORM_ROUND_STOREINT
using coordtype  = vec2<coordcomponent>;
using colourtype = vec4<coordcomponent>;

enum class aligntype : char {                                                   // the coordinate origin and size are relative to which part of the parent
  CENTRE,
  TOP,
  BOTTOM,
  LEFT,
  RIGHT,
  BOTTOM_LEFT,
  TOP_LEFT,
  BOTTOM_RIGHT,
  TOP_RIGHT
};

}

#endif // GUISTORM_TYPES_H_INCLUDED
