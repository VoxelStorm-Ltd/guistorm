#include "progressbar.h"
#include "gui.h"

namespace guistorm {

progressbar::progressbar(container *newparent,
                         colourset const &newcolours,
                         float thisvalue,
                         float thisscale,
                         coordtype const &thissize,
                         coordtype const &thisposition)
  : base(newparent, newcolours, "", nullptr, thissize, thisposition),
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
  if(__builtin_expect(vbo == 0, 0)) {                                           // if the buffer hasn't been generated yet (unlikely)
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
  #ifdef GUISTORM_UNBIND
    //glBindBuffer(GL_ARRAY_BUFFER,         0);
  #endif // GUISTORM_UNBIND
  glBindBuffer(GL_ARRAY_BUFFER,         vbo_fill);
  glBufferData(GL_ARRAY_BUFFER,         vbodata_fill.size() * sizeof(vertex), &vbodata_fill[0], GL_STATIC_DRAW);
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

void progressbar::render() {
  /// Draw this element
  if(!visible) {
    return;
  }
  if(__builtin_expect(!initialised, 0)) {                                       // if the buffer hasn't been initialised yet (unlikely)
    setup_buffer();
  }
  if(numverts != 0) {
    if(colours.current.background.a != 0.0f) {                                  // skip drawing fully transparent parts
      glBindBuffer(GL_ARRAY_BUFFER,         vbo_fill);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
      glVertexAttribPointer(parent_gui->attrib_coords,    2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::coords)));
      glVertexAttribPointer(parent_gui->attrib_texcoords, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::texcoords)));
      glUniform4f(parent_gui->uniform_colour,
                  colours.current.background.r,
                  colours.current.background.g,
                  colours.current.background.b,
                  colours.current.background.a);
      glDrawElements(GL_TRIANGLE_FAN, numverts, GL_UNSIGNED_INT, 0);            // background
    }
