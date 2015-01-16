#include "lineshape.h"
#include "gui.h"

namespace guistorm {

lineshape::lineshape(container *parent,
                     colourset const &colours,
                     std::vector<std::pair<coordtype, coordtype>> const &lines,
                     coordtype const &thisposition)
  : base(parent, colours, "", nullptr, coordtype(), thisposition) {
  /// Specific constructor
  upload_shape(lines);
}

lineshape::~lineshape() {
  /// Default destructor
}

void lineshape::upload_shape(std::vector<std::pair<coordtype, coordtype>> const &lines) {
  /// Process a vector of lines defined as pairs of coordinates into an efficient indexed vertex array
  #ifndef NDEBUG
    if(!parent_gui) {
      std::cout << "GUIStorm: ERROR: Function " << __PRETTY_FUNCTION__ << " called without a parent_gui assigned!" << std::endl;
      return;
    }
  #endif
  vbodata.reserve(lines.size() * 2);      // worst case size reservation

  for(std::pair<coordtype, coordtype> pair : lines) {
    std::vector<coordtype*> coords({&pair.first, &pair.second});      // quick hack to iterate through a pair
    for(auto const &coord : coords) {
      GLuint thisindex = 0;
      for(vertex const &v : vbodata) {      // search the vector for this point and try to get its index
        if(v.coords == *coord) {
          break;
        }
        ++thisindex;
      }
      // if we didn't find an index for this vertex then thisindex == vbodata.size() already which means we need to add a new entry
      if(thisindex == vbodata.size()) {
        vbodata.emplace_back(*coord);
      }
      ibodata.emplace_back(thisindex);
    }
  }
  numverts = ibodata.size();

  vbodata.shrink_to_fit();

  #ifdef DEBUG_GUISTORM
    std::cout << "GUIStorm: Lineshape indexer: " << lines.size() * 2 << " input verts reduced to " << vbodata.size() << " (" << ((lines.size() * 2) - vbodata.size()) * 100 / (lines.size() * 2) << "% sharing)" << std::endl;
  #endif // DEBUG_GUISTORM

  refresh();                                  // calculates the transform
}

base *lineshape::get_picked(coordtype const &cursor_position) {
  /// Return true if the mouse is over this element
  if(parent_gui->cursor == this) {
    return nullptr;                           // skip collision check if this is the current cursor, or we can't click anything
  }
  return base::get_picked(cursor_position);
}

void lineshape::init_buffer() {
  /// Generate the buffers for this object
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ibo);
  // skip initialising unused label buffers
}
void lineshape::destroy_buffer() {
  /// Clean up the buffers in preparation for exit or context switch
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ibo);
  // skip destroying unused label buffers
  vbo = 0;
  ibo = 0;
  numverts = 0;
  initialised = false;
}

void lineshape::setup_buffer() {
  /// Create or update the buffer for this element
  if(__builtin_expect(vbo == 0, 0)) {   // if the buffer hasn't been generated yet (unlikely)
    init_buffer();
  }

  coordtype const position_transformed(parent_gui->coord_transform(position));
  vbodata_shifted = vbodata_transformed;      // reuse the already allocated vector here
  for(vertex &v : vbodata_shifted) {
    v.coords += position_transformed;
  }
  numverts = ibodata.size();

  #ifdef DEBUG_GUISTORM
    std::cout << "GUIStorm: Lineshape: Uploading " << vbodata.size() << " " << sizeof(vertex) << "B verts, " << numverts << " indices to vbo ("
              << (vbodata.size() * sizeof(vertex)) << "B, "
              << (numverts * sizeof(GLuint)) << "B)" << std::endl;
  #endif // DEBUG_GUISTORM

  glBindBuffer(GL_ARRAY_BUFFER,         vbo);
  glBufferData(GL_ARRAY_BUFFER,         vbodata_shifted.size() * sizeof(vertex), &vbodata_shifted[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER,         0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, numverts               * sizeof(GLuint), &ibodata[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  initialised = true;
}

void lineshape::refresh() {
  /// Transform the coords to gui space
  vbodata_transformed = vbodata;              // use the cached vector to avoid reallocations
  coordtype const baseoffset(parent_gui->coord_transform(coordtype(0.0, 0.0)));
  for(vertex &v : vbodata_transformed) {
    v.coords = parent_gui->coord_transform(v.coords) - baseoffset;
  }
  setup_buffer();
}

void lineshape::render() {
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
