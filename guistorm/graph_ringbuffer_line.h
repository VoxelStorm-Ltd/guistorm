#ifndef GUISTORM_GRAPH_RINGBUFFER_LINE_H_INCLUDED
#define GUISTORM_GRAPH_RINGBUFFER_LINE_H_INCLUDED

#include "base.h"
#ifndef GUISTORM_SINGLETHREADED
  #include <shared_mutex>
#endif // GUISTORM_SINGLETHREADED
#include <boost/circular_buffer.hpp>

namespace guistorm {

class graph_ringbuffer_line : public base {
  /// A horizontal line graph populated from an iteraterable container
private:
  GLuint vbo_fill = 0;                                                          // vertex buffer for the fill, in format compatible with GL_TRIANGLES
  GLuint ibo_fill = 0;                                                          // index buffer for the fill
  GLuint numverts_fill = 0;                                                     // number of vertices in the fill buffer

  float min = 0.0;                                                              // the minimum value shown on the graph
  float max = 1.0;                                                              // the maximum value shown on the graph

  boost::circular_buffer<float> data;                                           // the set of individual graph points
  #ifndef GUISTORM_SINGLETHREADED
    mutable std::shared_mutex data_mutex;
  #endif // GUISTORM_SINGLETHREADED

public:
  graph_ringbuffer_line(container *parent,
                        colourset const &colours,
                        size_t num_entries,
                        float min = 0.0,
                        float max = 1.0,
                        coordtype const &size     = coordtype(),
                        coordtype const &position = coordtype());
protected:
  virtual ~graph_ringbuffer_line() override;

public:
  void init_buffer()    override final;
  void destroy_buffer() override final;
  void setup_buffer()   override final;
  void render()         override final;

  void set_min(float new_min);
  float const &get_min() const __attribute__((__const__));
  void set_max(float new_max);
  float const &get_max() const __attribute__((__const__));
  void set_min_and_max(float new_min, float new_max);
  void set_min_auto();
  void set_max_auto();
  void set_min_and_max_auto();

  void clear();
  void push(float value);
};

}

#endif // GUISTORM_GRAPH_RINGBUFFER_LINE_H_INCLUDED
