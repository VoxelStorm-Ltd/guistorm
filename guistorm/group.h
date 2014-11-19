#ifndef GROUP_H_INCLUDED
#define GROUP_H_INCLUDED

#include "window.h"
#include <string>

namespace guistorm {

class group : public window {
  /// A cut-down window that has only position data, no drawable elements
public:
  group(container *parent,
        coordtype const &size     = coordtype(),
        coordtype const &position = coordtype());
  virtual ~group() override;

  base *get_picked(coordtype const &cursor_position) override final;

  #ifndef DEBUG_GUISTORM
    void init_buffer() override final;
    void destroy_buffer() override final;
  protected:
    void setup_buffer() override final;
  public:
    void render() override final;
  #endif // DEBUG_GUISTORM
};

}

#endif // GROUP_H_INCLUDED
