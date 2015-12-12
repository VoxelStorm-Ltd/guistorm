#ifndef GUISTORM_BUTTON_H_INCLUDED
#define GUISTORM_BUTTON_H_INCLUDED

#include "widget.h"
#include <string>
#include <functional>

namespace guistorm {

class button : public widget {
public:
  std::function<void()> function;

  button(container *parent,
         colourset const &colours,
         std::string const &label = std::string(),
         std::function<void()> callback_function = nullptr,
         font *label_font = nullptr,
         coordtype const &size     = coordtype(),
         coordtype const &position = coordtype());
protected:
  virtual ~button() override;

public:
  //void on_press()   override final;
  void on_release() override final;

  void execute();
};

}

#endif // GUISTORM_BUTTON_H_INCLUDED
