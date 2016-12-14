  glGenBuffers(1, &vbo_fill);
  glGenBuffers(1, &ibo);
  glGenBuffers(1, &ibo_fill);
}
void graph_ringbuffer_line::destroy_buffer() {
  /// Clean up the buffers in preparation for exit or context switch
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &vbo_fill);
  glDeleteBuffers(1, &ibo);
  glDeleteBuffers(1, &ibo_fill);
  vbo           = 0;
#include "graph_ringbuffer_line.h"
  vbo_fill      = 0;
#include "cast_if_required.h"
  ibo           = 0;
#include <boost/algorithm/clamp.hpp>
  ibo_fill      = 0;
#include "gui.h"
  numverts      = 0;

  numverts_fill = 0;
namespace guistorm {
  initialised = false;

}
graph_ringbuffer_line::graph_ringbuffer_line(container *newparent,

                       colourset const &newcolours,
void graph_ringbuffer_line::setup_buffer() {
                       size_t num_entries,
  /// Create or update the buffer for this element
                       float thismin,
  if(data.empty()) {
                       float thismax,
    return;                                                                     // don't try to draw empty graphs
                       coordtype const &thissize,
  }
                       coordtype const &thisposition)
  if(__builtin_expect(vbo == 0, 0)) {                                           // if the buffer hasn't been generated yet (unlikely)
  : base(newparent, newcolours, "", nullptr, thissize, thisposition),
    init_buffer();
    min(thismin),
  }
    max(thismax),
  coordtype const position_absolute(get_absolute_position());
    data(num_entries) {
  std::vector<vertex> vbodata;
  /// Specific constructor
  focusable = false;
  std::vector<GLuint> ibodata;
}
  vbodata.reserve(data.size());
graph_ringbuffer_line::~graph_ringbuffer_line() {
  ibodata.reserve(data.size());
  /// Default destructor

}
  float const xstep = size.x / static_cast<float>(data.capacity());

  float vertical_scale;
void graph_ringbuffer_line::init_buffer() {
  if(max == min) {
  /// Generate the buffers for this object
    vertical_scale = 0.0;
  glGenBuffers(1, &vbo);
  } else {
  glGenBuffers(1, &vbo_fill);
    vertical_scale = size.y / (max - min);
  glGenBuffers(1, &ibo);
  }
  glGenBuffers(1, &ibo_fill);
  float x = position_absolute.x;
}
  for(auto const &it : data) {
void graph_ringbuffer_line::destroy_buffer() {
    ibodata.emplace_back(vbodata.size());
  /// Clean up the buffers in preparation for exit or context switch
  glDeleteBuffers(1, &vbo);
    float const val = position_absolute.y + boost::algorithm::clamp((it - min) * vertical_scale, 0.0f, size.y);
  glDeleteBuffers(1, &vbo_fill);
    vbodata.emplace_back(parent_gui->coord_transform(coordtype(x, val)));
  glDeleteBuffers(1, &ibo);
    x += xstep;
  glDeleteBuffers(1, &ibo_fill);
    // TODO: populate the fill buffer
  vbo           = 0;
  }
  vbo_fill      = 0;
  numverts = cast_if_required<GLuint>(ibodata.size());
  ibo           = 0;

  ibo_fill      = 0;
  glBindBuffer(GL_ARRAY_BUFFER,         vbo);
  numverts      = 0;
  glBufferData(GL_ARRAY_BUFFER,         vbodata.size() * sizeof(vertex), &vbodata[0], GL_STATIC_DRAW);
  numverts_fill = 0;
  #ifdef GUISTORM_UNBIND
  initialised = false;
    glBindBuffer(GL_ARRAY_BUFFER,         0);
}
  #endif // GUISTORM_UNBIND

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
void graph_ringbuffer_line::setup_buffer() {
  /// Create or update the buffer for this element
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, numverts       * sizeof(GLuint), &ibodata[0], GL_STATIC_DRAW);
  if(data.empty()) {
  #ifdef GUISTORM_UNBIND
    return;                                                                     // don't try to draw empty graphs
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }
  #endif // GUISTORM_UNBIND
  if(__builtin_expect(vbo == 0, 0)) {                                           // if the buffer hasn't been generated yet (unlikely)

  initialised = true;
}

    init_buffer();
void graph_ringbuffer_line::render() {
  }
  /// Draw this element
  coordtype const position_absolute(get_absolute_position());
  if(!visible) {
  std::vector<vertex> vbodata;
    return;
  std::vector<GLuint> ibodata;
  }
  vbodata.reserve(data.size());
  if(__builtin_expect(!initialised, 0)) {                                       // if the buffer hasn't been initialised yet (unlikely)
  ibodata.reserve(data.size());
    setup_buffer();

  }
  float const xstep = size.x / static_cast<float>(data.capacity());
  if(numverts != 0) {
  float vertical_scale;
    if(colours.current.background.a != 0.0f) {                                  // skip drawing fully transparent parts
  if(max == min) {
      glBindBuffer(GL_ARRAY_BUFFER,         vbo_fill);
    vertical_scale = 0.0;
  } else {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_fill);
    vertical_scale = size.y / (max - min);
      glVertexAttribPointer(parent_gui->attrib_coords,    2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::coords)));
  }
      glVertexAttribPointer(parent_gui->attrib_texcoords, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::texcoords)));
  float x = position_absolute.x;
      glUniform4f(parent_gui->uniform_colour,
  for(auto const &it : data) {
                  colours.current.background.r,
    ibodata.emplace_back(vbodata.size());
                  colours.current.background.g,
    float const val = position_absolute.y + boost::algorithm::clamp((it - min) * vertical_scale, 0.0f, size.y);
                  colours.current.background.b,
    vbodata.emplace_back(parent_gui->coord_transform(coordtype(x, val)));
                  colours.current.background.a);
    x += xstep;
      glDrawElements(GL_TRIANGLES, numverts_fill, GL_UNSIGNED_INT, 0);          // fill under the line
    // TODO: populate the fill buffer
    }
  }
    if(colours.current.content.a != 0.0f) {                                     // skip drawing fully transparent parts
  numverts = cast_if_required<GLuint>(ibodata.size());
      glBindBuffer(GL_ARRAY_BUFFER,         vbo);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBindBuffer(GL_ARRAY_BUFFER,         vbo);
  glBufferData(GL_ARRAY_BUFFER,         vbodata.size() * sizeof(vertex), &vbodata[0], GL_STATIC_DRAW);
      glVertexAttribPointer(parent_gui->attrib_coords,    2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::coords)));
  #ifdef GUISTORM_UNBIND
      glVertexAttribPointer(parent_gui->attrib_texcoords, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::texcoords)));
    glBindBuffer(GL_ARRAY_BUFFER,         0);
      glUniform4f(parent_gui->uniform_colour,
  #endif // GUISTORM_UNBIND
                  colours.current.content.r,
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
                  colours.current.content.g,
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, numverts       * sizeof(GLuint), &ibodata[0], GL_STATIC_DRAW);
                  colours.current.content.b,
  #ifdef GUISTORM_UNBIND
                  colours.current.content.a);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
      glDrawElements(GL_LINE_STRIP, numverts, GL_UNSIGNED_INT, 0);              // line
  #endif // GUISTORM_UNBIND
    }

  }
  initialised = true;

}
  update();

}
void graph_ringbuffer_line::render() {

  /// Draw this element
void graph_ringbuffer_line::set_min(float new_min) {
  if(!visible) {
  if(min != new_min) {
    return;
    min = new_min;
  }
    initialised = false;                                                        // mark the buffer as needing a refresh
  if(__builtin_expect(!initialised, 0)) {                                       // if the buffer hasn't been initialised yet (unlikely)
  }
    setup_buffer();
}
  }
float const &graph_ringbuffer_line::get_min() const {
  if(numverts != 0) {
  return min;
    if(colours.current.background.a != 0.0f) {                                  // skip drawing fully transparent parts
}
      glBindBuffer(GL_ARRAY_BUFFER,         vbo_fill);
void graph_ringbuffer_line::set_max(float new_max) {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_fill);
  if(max != new_max) {
      glVertexAttribPointer(parent_gui->attrib_coords,    2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::coords)));
    max = new_max;
      glVertexAttribPointer(parent_gui->attrib_texcoords, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::texcoords)));
    initialised = false;                                                        // mark the buffer as needing a refresh
      glUniform4f(parent_gui->uniform_colour,
  }
                  colours.current.background.r,
}
                  colours.current.background.g,
float const &graph_ringbuffer_line::get_max() const {
                  colours.current.background.b,
  return max;
                  colours.current.background.a);
}
      glDrawElements(GL_TRIANGLES, numverts_fill, GL_UNSIGNED_INT, 0);          // fill under the line
void graph_ringbuffer_line::set_min_and_max(float new_min, float new_max) {
    }
  set_min(new_min);
    if(colours.current.content.a != 0.0f) {                                     // skip drawing fully transparent parts
  set_max(new_max);
      glBindBuffer(GL_ARRAY_BUFFER,         vbo);
}
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

      glVertexAttribPointer(parent_gui->attrib_coords,    2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::coords)));
void graph_ringbuffer_line::set_min_auto() {
      glVertexAttribPointer(parent_gui->attrib_texcoords, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::texcoords)));
  /// Automatically scale the graph to fit the lowest element of the data
      glUniform4f(parent_gui->uniform_colour,
  if(__builtin_expect(data.empty(), 0)) {                                       // branch prediction hint: unlikely
                  colours.current.content.r,
    min = 0.0;
                  colours.current.content.g,
    return;
                  colours.current.content.b,
  }
                  colours.current.content.a);
  min = *std::min_element(data.begin(), data.end());
      glDrawElements(GL_LINE_STRIP, numverts, GL_UNSIGNED_INT, 0);              // line
}
    }
void graph_ringbuffer_line::set_max_auto() {
  }
  /// Automatically scale the graph to fit the highest element of the data

  update();
  if(__builtin_expect(data.empty(), 0)) {                                       // branch prediction hint: unlikely
}
    max = 0.0;

    return;
void graph_ringbuffer_line::set_min(float new_min) {
  }
  if(min != new_min) {
  max = *std::max_element(data.begin(), data.end());
    min = new_min;
}
    initialised = false;                                                        // mark the buffer as needing a refresh
void graph_ringbuffer_line::set_min_and_max_auto() {
  }
  /// Automatically scale the graph to fit all elements of the data
}
  if(__builtin_expect(data.empty(), 0)) {                                       // branch prediction hint: unlikely
float const &graph_ringbuffer_line::get_min() const {
    min = 0.0;
  return min;
    max = 0.0;
}
    return;
void graph_ringbuffer_line::set_max(float new_max) {
  }
  if(max != new_max) {
  auto minmax(std::minmax_element(data.begin(), data.end()));
    max = new_max;
  min = *minmax.first;
    initialised = false;                                                        // mark the buffer as needing a refresh
  max = *minmax.second;
  }
}
}

float const &graph_ringbuffer_line::get_max() const {
void graph_ringbuffer_line::clear() {
  return max;
  data.clear();
}
}
void graph_ringbuffer_line::set_min_and_max(float new_min, float new_max) {
void graph_ringbuffer_line::push(float value) {
  set_min(new_min);
  /// Upload a new data point to the graph
  set_max(new_max);
  data.push_back(value);
}

  initialised = false;                                                          // mark the buffer as needing a refresh
void graph_ringbuffer_line::set_min_auto() {
}
  /// Automatically scale the graph to fit the lowest element of the data

}
  if(__builtin_expect(data.empty(), 0)) {                                       // branch prediction hint: unlikely
    min = 0.0;
    return;
  }
  min = *std::min_element(data.begin(), data.end());
}
void graph_ringbuffer_line::set_max_auto() {
  /// Automatically scale the graph to fit the highest element of the data
  if(__builtin_expect(data.empty(), 0)) {                                       // branch prediction hint: unlikely
    max = 0.0;
    return;
  }
  max = *std::max_element(data.begin(), data.end());
}
void graph_ringbuffer_line::set_min_and_max_auto() {
  /// Automatically scale the graph to fit all elements of the data
  if(__builtin_expect(data.empty(), 0)) {                                       // branch prediction hint: unlikely
    min = 0.0;
    max = 0.0;
    return;
  }
  auto minmax(std::minmax_element(data.begin(), data.end()));
  min = *minmax.first;
  max = *minmax.second;
}

void graph_ringbuffer_line::clear() {
  data.clear();
}
void graph_ringbuffer_line::push(float value) {
  /// Upload a new data point to the graph
  data.push_back(value);
  initialised = false;                                                          // mark the buffer as needing a refresh
}

}
