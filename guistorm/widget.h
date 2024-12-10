#pragma once

#include "base.h"

namespace guistorm {

class widget : public base {
public:
  widget(container *parent,
         colourset const &colours,
         std::string const &label = std::string(),
         font *label_font = nullptr,
         coordtype const &size     = coordtype(),
         coordtype const &position = coordtype());
protected:
  ~widget();
};

}
