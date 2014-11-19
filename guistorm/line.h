#ifndef LINE_H_INCLUDED
#define LINE_H_INCLUDED

#include "base.h"

namespace guistorm {

class line : public base {
  /// Minimal gui element for drawing a 2D line, colour is outline
public:
  line(container *parent,
       colourset const &colours,
       coordtype const &size     = coordtype(),
       coordtype const &position = coordtype());
  ~line();

  void init_buffer() override final;
  void destroy_buffer() override final;
protected:
  void setup_buffer() override final;
public:
  void render() override final;
};

}


#endif // LINE_H_INCLUDED
