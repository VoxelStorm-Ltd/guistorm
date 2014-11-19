#include "widget.h"

namespace guistorm {

widget::widget(container *parent,
               colourset const &colours,
               std::string const &label,
               font *label_font,
               coordtype const &thissize,
               coordtype const &thisposition)
  : base(parent, colours, label, label_font, thissize, thisposition) {
  /// Specific constructor
}

widget::~widget() {
  /// Default destructor
}

}
