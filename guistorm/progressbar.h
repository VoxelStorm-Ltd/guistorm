#ifndef GUISTORM_PROGRESSBAR_H_INCLUDED
#define GUISTORM_PROGRESSBAR_H_INCLUDED

#include "base.h"
#include <vector>

namespace guistorm {

class progressbar : public base {
  /// A horizontal bar the fill of which reaches across horizontally to part of its width
private:
  GLuint vbo_fill = 0;                                                          // vertex buffer for the fill, in format compatible with GL_TRIANGLE_FAN - we use the same IBO as base

  float value = 0.0;                                                            // the value of the progress bar, from 0 to scale (may be negative if scale is negative)
  float scale = 1.0;                                                            // the scale of the progress bar, that the value relates to (may be negative)
public:
  progressbar(container *parent,
              colourset const &colours,
              float value = 0.0,
              float scale = 1.0,
              coordtype const &size     = coordtype(),
              coordtype const &position = coordtype());
protected:
  virtual ~progressbar() override;

public:
  void init_buffer()    override final;
  void destroy_buffer() override final;
  void setup_buffer()   override final;
  void render()         override final;

  void set_value(float new_value);
  float const &get_value() const __attribute__((__const__));
  void set_scale(float new_scale);
  float const &get_scale() const __attribute__((__const__));
  void set_value_and_scale(float new_value, float new_scale);
  void set_percentage(float new_percentage);
  float get_percentage() const __attribute__((__pure__));
};

}

#endif // GUISTORM_PROGRESSBAR_H_INCLUDED
