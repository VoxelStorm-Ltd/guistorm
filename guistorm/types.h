#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "vmath.h"

namespace guistorm {

// types
typedef Vector2<GLfloat> coordtype;
typedef Vector4<GLfloat> colourtype;

enum class aligntype : char {                 // the coordinate origin and size are relative to which part of the parent
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

#endif // TYPES_H_INCLUDED
