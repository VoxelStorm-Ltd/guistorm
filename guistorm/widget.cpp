#include "widget.h"

namespace guistorm {

widget::widget(container *newparent,
               colourset const &newcolours,
               std::string const &newlabel,
               font *newlabel_font,
               coordtype const &thissize,
               coordtype const &thisposition)
  : base(newparent, newcolours, newlabel, newlabel_font, thissize, thisposition) {
  /// Specific constructor
}

widget::~widget() {
  /// Default destructor
}

}
