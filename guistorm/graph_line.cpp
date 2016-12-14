#include "graph_line.h"
#include <boost/algorithm/clamp.hpp>
#include "cast_if_required.h"
#include "gui.h"

namespace guistorm {

graph_line::graph_line(container *newparent,
                       colourset const &newcolours,
                       float thismin,
                       float thismax,
                       coordtype const &thissize,
                       coordtype const &thisposition)
  : base(newparent, newcolours, "", nullptr, thissize, thisposition),
    min(thismin),
    max(thismax) {
  /// Specific constructor
  focusable = false;
}
graph_line::~graph_line() {
  /// Default destructor
}

void graph_line::init_buffer() {
  /// Generate the buffers for this object
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &vbo_fill);
  glGenBuffers(1, &ibo);
  glGenBuffers(1, &ibo_fill);
}
void graph_line::destroy_buffer() {
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

void graph_line::setup_buffer() {
  /// Create or update the buffer for this element
  if(__builtin_expect(vbo == 0, 0)) {                                           // if the buffer hasn't been generated yet (unlikely)
    init_buffer();
  }
  coordtype const position_absolute(get_absolute_position());
  std::vector<vertex> vbodata;
  std::vector<GLuint> ibodata;
      glBindBuffer(GL_ARRAY_BUFFER,         vbo_fill);
  vbodata.reserve(data.size());
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_fill);
  ibodata.reserve(data.size());
      glVertexAttribPointer(parent_gui->attrib_coords,    2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::coords)));

  float const xstep = size.x / static_cast<float>(data.size());
      glVertexAttribPointer(parent_gui->attrib_texcoords, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::texcoords)));
  float vertical_scale;
      glUniform4f(parent_gui->uniform_colour,
  if(max == min) {
                  colours.current.background.r,
    vertical_scale = 0.0;
                  colours.current.background.g,
  } else {
                  colours.current.background.b,
    vertical_scale = size.y / (max - min);
                  colours.current.background.a);
  }
      glDrawElements(GL_TRIANGLES, numverts_fill, GL_UNSIGNED_INT, 0);          // fill under the line
  float x = position_absolute.x;
    }
  for(auto const &it : data) {
    if(colours.current.content.a != 0.0f) {                                     // skip drawing fully transparent parts
    ibodata.emplace_back(vbodata.size());
      glBindBuffer(GL_ARRAY_BUFFER,         vbo);
    float const val = position_absolute.y + boost::algorithm::clamp((it - min) * vertical_scale, 0.0f, size.y);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    vbodata.emplace_back(parent_gui->coord_transform(coordtype(x, val)));
      glVertexAttribPointer(parent_gui->attrib_coords,    2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::coords)));
    x += xstep;
      glVertexAttribPointer(parent_gui->attrib_texcoords, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::texcoords)));
    // TODO: populate the fill buffer
      glUniform4f(parent_gui->uniform_colour,
  }
                  colours.current.content.r,
  numverts = cast_if_required<GLuint>(ibodata.size());
                  colours.current.content.g,

                  colours.current.content.b,
  glBindBuffer(GL_ARRAY_BUFFER,         vbo);
                  colours.current.content.a);
  glBufferData(GL_ARRAY_BUFFER,         vbodata.size() * sizeof(vertex), &vbodata[0], GL_STATIC_DRAW);
      glDrawElements(GL_LINE_STRIP, numverts, GL_UNSIGNED_INT, 0);              // line
  #ifdef GUISTORM_UNBIND
    }
    glBindBuffer(GL_ARRAY_BUFFER,         0);
  }
  #endif // GUISTORM_UNBIND

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  update();
}
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, numverts       * sizeof(GLuint), &ibodata[0], GL_STATIC_DRAW);

  #ifdef GUISTORM_UNBIND
void graph_line::set_min(float new_min) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  if(min != new_min) {
  #endif // GUISTORM_UNBIND
    min = new_min;

    initialised = false;                                                        // mark the buffer as needing a refresh
  initialised = true;
  }
}
}

float const &graph_line::get_min() const {
void graph_line::render() {
  return min;
  /// Draw this element
}
  if(!visible) {
void graph_line::set_max(float new_max) {
    return;
  if(max != new_max) {
  }
    max = new_max;
  if(__builtin_expect(!initialised, 0)) {                                       // if the buffer hasn't been initialised yet (unlikely)
    initialised = false;                                                        // mark the buffer as needing a refresh
    setup_buffer();
  }
  if(numverts != 0) {
}
    if(colours.current.background.a != 0.0f) {                                  // skip drawing fully transparent parts
float const &graph_line::get_max() const {
  return max;
}
void graph_line::set_min_and_max(float new_min, float new_max) {
  set_min(new_min);
  set_max(new_max);
}

void graph_line::set_min_auto() {
  /// Automatically scale the graph to fit the lowest element of the data
  if(__builtin_expect(data.empty(), 0)) {                                       // branch prediction hint: unlikely
    min = 0.0;
    return;
  }
  min = *std::min_element(data.begin(), data.end());
}
void graph_line::set_max_auto() {
  /// Automatically scale the graph to fit the highest element of the data
  if(__builtin_expect(data.empty(), 0)) {                                       // branch prediction hint: unlikely
    max = 0.0;
    return;
  }
  max = *std::max_element(data.begin(), data.end());
}
void graph_line::set_min_and_max_auto() {
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

}
