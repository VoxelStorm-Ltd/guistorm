#ifndef GUISTORM_COLOURGROUP_H_INCLUDED
#define GUISTORM_COLOURGROUP_H_INCLUDED

#include "types.h"

namespace guistorm {

class colourgroup {
public:
  colourtype background;                                                        // colour of the solid fill of this element
  colourtype outline;                                                           // colour of the outline box of this element
  colourtype content;                                                           // colour of the text content of this element

  colourgroup();
  colourgroup(colourtype const &background,
              colourtype const &outline,
              colourtype const &content);
  ~colourgroup() __attribute__((__const__));

  void assign(colourtype const &new_background,
              colourtype const &new_outline,
              colourtype const &new_content);

  void blend_to(colourgroup const &target, float factor);
};

}

#endif // GUISTORM_COLOURGROUP_H_INCLUDED
