#ifndef WINDOW_H_INCLUDED
#define WINDOW_H_INCLUDED

#include "base.h"
#include "container.h"

namespace guistorm {

class window : public base, public container {
public:
  bool capture_click = true;

  window(container *parent,
         colourset const &colours,
         std::string const &label = std::string(),
         font *label_font = nullptr,
         coordtype const &size     = coordtype(),
         coordtype const &position = coordtype());
protected:
  virtual ~window() override;

public:
  void add_to_gui(base *element) override final;

  base *get_picked(coordtype const &cursor_position) override;
  coordtype const get_absolute_position() const override final;

  // layout control
  void stretch_vertical(  GLfloat margin = 0.0);
  void stretch_vertical(  std::vector<base*>::const_iterator first, std::vector<base*>::const_iterator last, GLfloat margin = 0.0);
  void stretch_horizontal(GLfloat margin = 0.0);
  void stretch_horizontal(std::vector<base*>::const_iterator first, std::vector<base*>::const_iterator last, GLfloat margin = 0.0);
  void layout_vertical(   GLfloat margin = 0.0, aligntype alignment = aligntype::CENTRE);
  void layout_vertical(   coordtype const &bottomleft, coordtype const &topright, aligntype alignment = aligntype::CENTRE);
  void layout_vertical(   std::vector<base*>::const_iterator first, std::vector<base*>::const_iterator last, GLfloat margin = 0.0, aligntype alignment = aligntype::CENTRE);
  void layout_vertical(   std::vector<base*>::const_iterator first, std::vector<base*>::const_iterator last, coordtype const &bottomleft = coordtype(), coordtype const &topright = coordtype(), aligntype alignment = aligntype::CENTRE);
  void layout_horizontal( GLfloat margin = 0.0, aligntype alignment = aligntype::CENTRE);
  void layout_horizontal( coordtype const &bottomleft, coordtype const &topright, aligntype alignment = aligntype::CENTRE);
  void layout_horizontal( std::vector<base*>::const_iterator first, std::vector<base*>::const_iterator last, GLfloat margin = 0.0, aligntype alignment = aligntype::CENTRE);
  void layout_horizontal( std::vector<base*>::const_iterator first, std::vector<base*>::const_iterator last, coordtype const &bottomleft = coordtype(), coordtype const &topright = coordtype(), aligntype alignment = aligntype::CENTRE);

  void destroy_buffer() override;

  void update_layout() override final;
  void refresh() override final;

  void render() override;
};

}

#endif // WINDOW_H_INCLUDED
