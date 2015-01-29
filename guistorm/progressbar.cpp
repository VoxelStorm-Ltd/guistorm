#include "progressbar.h"
#include "gui.h"

namespace guistorm {

progressbar::progressbar(container *parent,
                         colourset const &colours,
                         float thisvalue,
                         float thisscale,
                         coordtype const &thissize,
                         coordtype const &thisposition)
  : base(parent, colours, "", nullptr, thissize, thisposition),
    value(thisvalue),
    scale(thisscale) {
  /// Specific constructor
  focusable = false;
}
progressbar::~progressbar() {
  /// Default destructor
}

void progressbar::init_buffer() {
  /// Generate the buffers for this object
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &vbo_fill);
  glGenBuffers(1, &ibo);
}
void progressbar::destroy_buffer() {
  /// Clean up the buffers in preparation for exit or context switch
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &vbo_fill);
  glDeleteBuffers(1, &ibo);
  vbo      = 0;
  vbo_fill = 0;
  ibo      = 0;
  numverts = 0;
  initialised = false;
}

void progressbar::setup_buffer() {
  /// Create or update the buffer for this element
  if(__builtin_expect(vbo == 0, 0)) {  // if the buffer hasn't been generated yet (unlikely)
    init_buffer();
  }
  coordtype const position_absolute(get_absolute_position());
  std::vector<vertex> vbodata;
  vbodata.reserve(4);
  vbodata.emplace_back(parent_gui->coord_transform(coordtype(position_absolute.x         , position_absolute.y         )));
  vbodata.emplace_back(parent_gui->coord_transform(coordtype(position_absolute.x + size.x, position_absolute.y         )));
  vbodata.emplace_back(parent_gui->coord_transform(coordtype(position_absolute.x + size.x, position_absolute.y + size.y)));
  vbodata.emplace_back(parent_gui->coord_transform(coordtype(position_absolute.x         , position_absolute.y + size.y)));
  std::vector<vertex> vbodata_fill;
  vbodata_fill.reserve(4);
  vbodata_fill.emplace_back(parent_gui->coord_transform(coordtype(position_absolute.x                           , position_absolute.y         )));
  vbodata_fill.emplace_back(parent_gui->coord_transform(coordtype(position_absolute.x + size.x * (value / scale), position_absolute.y         )));
  vbodata_fill.emplace_back(parent_gui->coord_transform(coordtype(position_absolute.x + size.x * (value / scale), position_absolute.y + size.y)));
  vbodata_fill.emplace_back(parent_gui->coord_transform(coordtype(position_absolute.x                           , position_absolute.y + size.y)));
  std::vector<GLuint> ibodata{0, 1, 2, 3};
  numverts = 4;

  glBindBuffer(GL_ARRAY_BUFFER,         vbo);
  glBufferData(GL_ARRAY_BUFFER,         vbodata.size() * sizeof(vertex), &vbodata[0], GL_STATIC_DRAW);
  //glBindBuffer(GL_ARRAY_BUFFER,         0);
  glBindBuffer(GL_ARRAY_BUFFER,         vbo_fill);
  glBufferData(GL_ARRAY_BUFFER,         vbodata_fill.size() * sizeof(vertex), &vbodata_fill[0], GL_STATIC_DRAW);
  //glBindBuffer(GL_ARRAY_BUFFER,         0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, numverts       * sizeof(GLuint), &ibodata[0], GL_STATIC_DRAW);
  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  initialised = true;
}

void progressbar::render() {
  /// Draw this element
  if(!visible) {
    return;
  }
  if(__builtin_expect(!initialised, 0)) {  // if the buffer hasn't been initialised yet (unlikely)
    setup_buffer();
  }
  if(numverts != 0) {
    if(colours.current.background.a != 0.0f) {                          // skip drawing fully transparent parts
      glBindBuffer(GL_ARRAY_BUFFER,         vbo_fill);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
      glVertexAttribPointer(parent_gui->attrib_coords,    2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::coords)));
      glVertexAttribPointer(parent_gui->attrib_texcoords, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::texcoords)));
      glUniform4f(parent_gui->uniform_colour,
                  colours.current.background.r,
                  colours.current.background.g,
                  colours.current.background.b,
                  colours.current.background.a);
      glDrawElements(GL_TRIANGLE_FAN, numverts, GL_UNSIGNED_INT, 0);    // background
    }
    if(colours.current.outline.a != 0.0f) {                             // skip drawing fully transparent parts
      glBindBuffer(GL_ARRAY_BUFFER,         vbo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
      glVertexAttribPointer(parent_gui->attrib_coords,    2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::coords)));
      glVertexAttribPointer(parent_gui->attrib_texcoords, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::texcoords)));
      glUniform4f(parent_gui->uniform_colour,
                  colours.current.outline.r,
                  colours.current.outline.g,
                  colours.current.outline.b,
                  colours.current.outline.a);
      glDrawElements(GL_LINE_LOOP,    numverts, GL_UNSIGNED_INT, 0);    // outline
    }
  }

  update();
}

void progressbar::set_value(float new_value) {
  /// update the value displayed by this progress bar, and refresh the buffer if it's changed
  if(value == new_value) {
    return;
  }
  value = new_value;
  setup_buffer();
}
float const &progressbar::get_value() const {
  return value;
}

void progressbar::set_scale(float new_scale) {
  /// update the scale on which the progress bar displays, and refresh the buffer if it's changed
  if(scale == new_scale) {
    return;
  }
  scale = new_scale;
  setup_buffer();
}
float const &progressbar::get_scale() const {
  return scale;
}
void progressbar::set_value_and_scale(float new_value, float new_scale) {
  /// Convenience function to avoid double buffer adjusts
  if(scale == new_scale && value == new_value) {
    return;
  }
  value = new_value;
  scale = new_scale;
  setup_buffer();
}
void progressbar::set_percentage(float new_percentage) {
  /// Wrapper for dealing with percentages
  set_value(new_percentage / 100.0f);
}
float progressbar::get_percentage() const {
  /// Wrapper for dealing with percentages
  return get_value() * 100.0f;
}

}
