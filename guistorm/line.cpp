#include "line.h"
#include "gui.h"
#include "container.h"

namespace guistorm {

line::line(container *parent,
           colourset const &colours,
           coordtype const &thissize,
           coordtype const &thisposition)
  : base(parent, colours, "", nullptr, thissize, thisposition) {
  /// Specific constructor
  focusable = false;
}

line::~line() {
  /// Default destructor
}

void line::init_buffer() {
  /// Generate the buffers for this object
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ibo);
  // skip initialising unused label buffers
}
void line::destroy_buffer() {
  /// Clean up the buffers in preparation for exit or context switch
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ibo);
  // skip destroying unused label buffers
  vbo = 0;
  ibo = 0;
  numverts = 0;
  initialised = false;
}

void line::setup_buffer() {
  /// Create or update the buffer for this element
  if(__builtin_expect(vbo == 0, 0)) {  // if the buffer hasn't been generated yet (unlikely)
    init_buffer();
  }

  coordtype const &position_absolute(get_absolute_position());

  std::vector<vertex> vbodata;
  vbodata.reserve(2);
  vbodata.emplace_back(parent_gui->coord_transform(coordtype(position_absolute.x         , position_absolute.y         )));
  vbodata.emplace_back(parent_gui->coord_transform(coordtype(position_absolute.x + size.x, position_absolute.y + size.y)));
  std::vector<GLuint> ibodata{0, 1};
  numverts = 2;

  glBindBuffer(GL_ARRAY_BUFFER,         vbo);
  glBufferData(GL_ARRAY_BUFFER,         vbodata.size() * sizeof(vertex), &vbodata[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER,         0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, numverts       * sizeof(GLuint), &ibodata[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  initialised = true;
}

void line::render() {
  /// Draw this element
  if(!visible) {
    return;
  }
  if(__builtin_expect(!initialised, 0)) {  // if the buffer hasn't been initialised yet (unlikely)
    setup_buffer();
  }
  glBindBuffer(GL_ARRAY_BUFFER,         vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glVertexAttribPointer(parent_gui->attrib_coords,    2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::coords)));
  glVertexAttribPointer(parent_gui->attrib_texcoords, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::texcoords)));

  glUniform4f(parent_gui->uniform_colour,
              colours.current.outline.r,
              colours.current.outline.g,
              colours.current.outline.b,
              colours.current.outline.a);
  glDrawElements(GL_LINES, numverts, GL_UNSIGNED_INT, 0);    // outline

  update();
}

}
