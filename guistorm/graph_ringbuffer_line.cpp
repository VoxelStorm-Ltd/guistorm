#include "graph_ringbuffer_line.h"
#include "cast_if_required.h"
#include "gui.h"

namespace guistorm {

graph_ringbuffer_line::graph_ringbuffer_line(container *newparent,
                       colourset const &newcolours,
                       size_t num_entries,
                       float thismin,
                       float thismax,
                       coordtype const &thissize,
                       coordtype const &thisposition)
  : base(newparent, newcolours, "", nullptr, thissize, thisposition),
    min(thismin),
    max(thismax),
    data(num_entries) {
  /// Specific constructor
  focusable = false;
}
graph_ringbuffer_line::~graph_ringbuffer_line() {
  /// Default destructor
}

void graph_ringbuffer_line::init_buffer() {
  /// Generate the buffers for this object
  glGenBuffers(1, &vbo);
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
  vbo_fill      = 0;
  ibo           = 0;
  ibo_fill      = 0;
  numverts      = 0;
  numverts_fill = 0;
  initialised = false;
}

void graph_ringbuffer_line::setup_buffer() {
  /// Create or update the buffer for this element
  #ifndef GUISTORM_SINGLETHREADED
    std::shared_lock<std::shared_mutex> lock(data_mutex);                       // lock for reading (shared)
  #endif // GUISTORM_SINGLETHREADED
  if(data.empty()) {
    initialised = true;
    return;                                                                     // don't try to draw empty graphs
  }
  #ifndef GUISTORM_SINGLETHREADED
    lock.unlock();
  #endif // GUISTORM_SINGLETHREADED
  if(__builtin_expect(vbo == 0, 0)) {                                           // if the buffer hasn't been generated yet (unlikely)
    init_buffer();
  }
  coordtype const position_absolute(get_absolute_position());
  std::vector<vertex> vbodata;
  std::vector<GLuint> ibodata;
  vbodata.reserve(data.size());
  ibodata.reserve(data.size());

  #ifndef GUISTORM_SINGLETHREADED
    lock.lock();
  #endif // GUISTORM_SINGLETHREADED
  float const xstep = size.x / static_cast<float>(data.capacity());
  float vertical_scale;
  if(max == min) {
    vertical_scale = 0.0;
  } else {
    vertical_scale = size.y / (max - min);
  }
  float x = position_absolute.x;
  for(auto const &it : data) {
    ibodata.emplace_back(vbodata.size());
    float const val = position_absolute.y + std::clamp((it - min) * vertical_scale, 0.0f, size.y);
    vbodata.emplace_back(parent_gui->coord_transform(coordtype(x, val)));
    x += xstep;
    // TODO: populate the fill buffer
  }
  #ifndef GUISTORM_SINGLETHREADED
    lock.unlock();
  #endif // GUISTORM_SINGLETHREADED
  numverts = cast_if_required<GLuint>(ibodata.size());

  glBindBuffer(GL_ARRAY_BUFFER,         vbo);
  glBufferData(GL_ARRAY_BUFFER,         vbodata.size() * sizeof(vertex), &vbodata[0], GL_STATIC_DRAW);
  #ifdef GUISTORM_UNBIND
    glBindBuffer(GL_ARRAY_BUFFER,         0);
  #endif // GUISTORM_UNBIND
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, numverts       * sizeof(GLuint), &ibodata[0], GL_STATIC_DRAW);
  #ifdef GUISTORM_UNBIND
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  #endif // GUISTORM_UNBIND

  initialised = true;
}

void graph_ringbuffer_line::render() {
  /// Draw this element
  if(!visible) {
    return;
  }
  if(!initialised) {                                                            // if the buffer hasn't been initialised yet (unlikely)
    setup_buffer();
  }
  if(numverts != 0) {
    if(colours.current.background.a != 0.0f) {                                  // skip drawing fully transparent parts
      glBindBuffer(GL_ARRAY_BUFFER,         vbo_fill);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_fill);
      glVertexAttribPointer(parent_gui->attrib_coords,    2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::coords)));
      glVertexAttribPointer(parent_gui->attrib_texcoords, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::texcoords)));
      glUniform4f(parent_gui->uniform_colour,
                  colours.current.background.r,
                  colours.current.background.g,
                  colours.current.background.b,
                  colours.current.background.a);
      glDrawElements(GL_TRIANGLES, numverts_fill, GL_UNSIGNED_INT, 0);          // fill under the line
    }
    if(colours.current.content.a != 0.0f) {                                     // skip drawing fully transparent parts
      glBindBuffer(GL_ARRAY_BUFFER,         vbo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
      glVertexAttribPointer(parent_gui->attrib_coords,    2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::coords)));
      glVertexAttribPointer(parent_gui->attrib_texcoords, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::texcoords)));
      glUniform4f(parent_gui->uniform_colour,
                  colours.current.content.r,
                  colours.current.content.g,
                  colours.current.content.b,
                  colours.current.content.a);
      glDrawElements(GL_LINE_STRIP, numverts, GL_UNSIGNED_INT, 0);              // line
    }
  }

  update();
}

void graph_ringbuffer_line::set_min(float new_min) {
  if(min != new_min) {
    min = new_min;
    initialised = false;                                                        // mark the buffer as needing a refresh
  }
}
float const &graph_ringbuffer_line::get_min() const {
  return min;
}
void graph_ringbuffer_line::set_max(float new_max) {
  if(max != new_max) {
    max = new_max;
    initialised = false;                                                        // mark the buffer as needing a refresh
  }
}
float const &graph_ringbuffer_line::get_max() const {
  return max;
}
void graph_ringbuffer_line::set_min_and_max(float new_min, float new_max) {
  set_min(new_min);
  set_max(new_max);
}

void graph_ringbuffer_line::set_min_auto() {
  /// Automatically scale the graph to fit the lowest element of the data
  #ifndef GUISTORM_SINGLETHREADED
    std::shared_lock<std::shared_mutex> lock(data_mutex);                       // lock for reading (shared)
  #endif // GUISTORM_SINGLETHREADED
  if(__builtin_expect(data.empty(), 0)) {                                       // branch prediction hint: unlikely
    min = 0.0;
    return;
  }
  min = *std::min_element(data.begin(), data.end());
}
void graph_ringbuffer_line::set_max_auto() {
  /// Automatically scale the graph to fit the highest element of the data
  #ifndef GUISTORM_SINGLETHREADED
    std::shared_lock<std::shared_mutex> lock(data_mutex);                       // lock for reading (shared)
  #endif // GUISTORM_SINGLETHREADED
  if(__builtin_expect(data.empty(), 0)) {                                       // branch prediction hint: unlikely
    max = 0.0;
    return;
  }
  max = *std::max_element(data.begin(), data.end());
}
void graph_ringbuffer_line::set_min_and_max_auto() {
  /// Automatically scale the graph to fit all elements of the data
  #ifndef GUISTORM_SINGLETHREADED
    std::shared_lock<std::shared_mutex> lock(data_mutex);                       // lock for reading (shared)
  #endif // GUISTORM_SINGLETHREADED
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
  #ifndef GUISTORM_SINGLETHREADED
    std::unique_lock<std::shared_mutex> lock(data_mutex);                       // lock for writing (unique)
  #endif // GUISTORM_SINGLETHREADED
  data.clear();
}
void graph_ringbuffer_line::push(float value) {
  /// Upload a new data point to the graph
  #ifndef GUISTORM_SINGLETHREADED
    std::unique_lock<std::shared_mutex> lock(data_mutex);                       // lock for writing (unique)
  #endif // GUISTORM_SINGLETHREADED
  data.push_back(value);
  initialised = false;                                                          // mark the buffer as needing a refresh
}

}
