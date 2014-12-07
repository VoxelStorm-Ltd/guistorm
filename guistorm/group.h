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
protected:
  virtual ~group() override;

public:
  base *get_picked(coordtype const &cursor_position) override final;

  #ifndef NDEBUG
    void set_label(std::string const &newlabel) override final;
  #endif
};

}

#endif // GROUP_H_INCLUDED
