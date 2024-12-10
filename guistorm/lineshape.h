#pragma once

#include "base.h"
#include <vector>

namespace guistorm {

class lineshape : public base {
  /// An object comprised of a set of straight vector lines
private:
  std::vector<vertex> vbodata;                                                  // the VBO vertex data, positioned relative to origin ready for shift and upload
  std::vector<vertex> vbodata_transformed;                                      // the cached VBO vertex data that's been transformed to gui space
  std::vector<vertex> vbodata_shifted;                                          // the cached VBO vertex data that's been offset by the object's position, ready for upload
  std::vector<GLuint> ibodata;                                                  // indices for the VBO

public:
  lineshape(container *parent,
            colourset const &colours,
            std::vector<std::pair<coordtype, coordtype>> const &lines,
            coordtype const &thisposition = coordtype(0.0f, 0.0f));
protected:
  virtual ~lineshape() override;

public:
  void upload_shape(std::vector<std::pair<coordtype, coordtype>> const &lines);

  base *get_picked(coordtype const &cursor_position) override final;

  void init_buffer() override final;
  void destroy_buffer() override final;
protected:
  void setup_buffer() override final;
public:
  void refresh() override final;
  void render() override final;
};

}
