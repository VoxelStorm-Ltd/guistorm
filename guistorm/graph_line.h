#ifndef GUISTORM_GRAPH_LINE_H_INCLUDED
#define GUISTORM_GRAPH_LINE_H_INCLUDED

#include "base.h"
#include <vector>
#include <boost/range/iterator_range.hpp>

namespace guistorm {

class graph_line : public base {
  /// A horizontal line graph populated from an iteraterable container
private:
  GLuint vbo_fill = 0;                      // vertex buffer for the fill, in format compatible with GL_TRIANGLES
  GLuint ibo_fill = 0;                      // index buffer for the fill
  GLuint numverts_fill = 0;                 // number of vertices in the fill buffer

  float min = 0.0;                          // the value of the progress bar, from 0 to scale (may be negative if scale is negative)
  float max = 1.0;                          // the scale of the progress bar, that the value relates to (may be negative)

  std::vector<float> data;                  // the set of individual graph points

public:
  graph_line(container *parent,
              colourset const &colours,
              float min = 0.0,
              float max = 1.0,
              coordtype const &size     = coordtype(),
              coordtype const &position = coordtype());
protected:
  virtual ~graph_line() override;

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

  template<typename T> void upload(T const &begin, T const &end);
};

template<typename T> void graph_line::upload(T const &begin, T const &end) {
  /// Upload a new set of data points to this graph
  data.clear();
  for(auto const &it : boost::make_iterator_range(begin, end)) {
    data.emplace_back(it);
  }
  initialised = false;                                                // mark the buffer as needing a refresh
}

}

#endif // GUISTORM_GRAPH_LINE_H_INCLUDED
