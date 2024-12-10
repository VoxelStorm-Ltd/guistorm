#pragma once

/// Forward declaration header, for inclusion in other headers when using pointers

namespace guistorm {
  class gui;

  class button;
  class graph_line;
  class graph_ringbuffer_line;
  class group;
  class line;
  class lineshape;
  class progressbar;
  class widget;
  class window;
  #ifndef GUISTORM_NO_TEXT
    class input_text;
    class label;
  #endif // GUISTORM_NO_TEXT

  class colourgroup;
  class colourset;
  #ifndef GUISTORM_NO_TEXT
    class font;
  #endif // GUISTORM_NO_TEXT
}
