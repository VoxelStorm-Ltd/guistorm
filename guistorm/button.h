#ifndef BUTTON_H_INCLUDED
#define BUTTON_H_INCLUDED

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
  virtual ~button() override;

  //void on_press()   override final;
  void on_release() override final;

  void execute();
};

}

#endif // BUTTON_H_INCLUDED
