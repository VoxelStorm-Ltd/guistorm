#pragma once

#ifndef GUISTORM_NO_TEXT

#include "widget.h"
#include <string>

namespace guistorm {

class label : public widget {
  /// A cut-down widget that has only a text label, no drawable outline buffer
public:
  label(container *parent,
        colourset const &colours,
        std::string const &label = std::string(),
        font *label_font = nullptr,
        coordtype const &size     = coordtype(),
        coordtype const &position = coordtype());
protected:
  virtual ~label() override;

public:
  base *get_picked(coordtype const &cursor_position) override final __attribute__((__const__));

  #ifndef DEBUG_GUISTORM
    void init_buffer() override final;
  protected:
    void destroy_buffer() override final;
  public:
    void setup_buffer() override final;
  #endif
};

}

#endif // GUISTORM_NO_TEXT
