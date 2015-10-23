#ifndef GUISTORM_BASE_H_INCLUDED
#define GUISTORM_BASE_H_INCLUDED

#include <vector>
#include "types.h"
#include "colourset.h"
#include "font.h"
#include "layout_rules.h"

namespace guistorm {

class gui;                                      // forward declarations
class container;

class base {
  friend class container;                       // needed to allow deletion by container
protected:
  // OpenGL rendering
  struct vertex {
    coordtype coords;
    coordtype texcoords;
    vertex(coordtype const &new_coords,
           coordtype const &new_texcoords = coordtype(1.0, 1.0))
    : coords(new_coords),
      texcoords(new_texcoords) {
      /// Specific constructor
    }
  };
  GLuint vbo            = 0;                    // vertex buffer, in format compatible with both GL_LINE_LOOP and GL_TRIANGLE_FAN
  GLuint ibo            = 0;                    // index buffer
  GLuint vbo_label      = 0;                    // vertex buffer for label text in GL_QUADS format
  GLuint ibo_label      = 0;                    // index buffer for label text
  GLuint numverts       = 0;                    // number of vertices to render for the main shape
  GLuint numverts_label = 0;                    // number of vertices to render for the label
  //GLuint font_atlas_id  = 0;                    // font atlas ID for this label

public:
  // relations
  gui *parent_gui   = nullptr;                  // the top level gui this entity is rendered as a part of
  container *parent = nullptr;                  // the container this object belongs to

protected:
  // state
  bool visible     = true;                      // whether to render this element
  bool initialised = false;                     // whether the buffer is populated and it's ready to be rendered
public:
  bool focusable = false;                       // whether it can be given focus
  bool focused   = false;                       // true recursively for all windows upwards from the currently focused widget or window
protected:
  bool mouseover = false;                       // only works on focusable items
  bool active    = false;                       // clicked - only true while clicking / typing / dragging

protected:
  // positions
  coordtype position;                           // the relative coordinates of this to its parent
  coordtype size;                               // the distance of the furthest corner

public:
  // colours
  colourset colours;                            // set of colours for different states of this object

  // label
private:
  std::string label_text;                       // the text for the label of this object
  std::vector<font::line> label_lines;          // the actual organised label content
  coordtype label_origin;                       // where to reset the pen to
  coordtype label_size;                         // maximum size of the label, width and height
  GLfloat label_line_spacing = 0.0;             // how far apart the label lines are vertically

public:
  aligntype label_alignment = aligntype::CENTRE;
  coordtype label_margin;                       // how far the label is from the alignment edge, or how big a distance to leave on both sides justifying
  bool label_merge_whitespace   = true;         // whether to collapse adjacent whitespace together into a single space
  bool label_merge_newlines     = false;        // whether to collapse adjacent line breaks together into a single newline
  bool label_wordwrap           = true;         // whether to wrap words to new lines when they exceed the width of the containing box
  bool label_justify_horizontal = true;         // whether to justify each line horizontally
  bool label_justify_vertical   = false;        // whether to justify the whole text to fill the vertical space
  bool label_stretch_vertical   = false;        // whether to stretch the container vertically to fit the label if the label is taller
  bool label_shrink_vertical    = false;        // whether to shrink the container vertically to fit the label if the label is shorter
  font *label_font = nullptr;                   // the font chosen for this label - this may be nullptr, use get_label_font() to get one safely

  // layout rules
  std::vector<layout::rule> layout_rules;       // container for layout rules in the order in which they're applied to determine the element's position

protected:
  base(container *parent,
       colourset const &colours,
       std::string const &label,
       font *label_font = nullptr,
       coordtype const &size     = coordtype(),
       coordtype const &position = coordtype());
  virtual ~base();
public:
  static void operator delete(void *p);

public:
  // control
  void show();
  void hide();
  void toggle();
  void set_position(coordtype const &new_position);
  void set_position(GLfloat new_position_x, GLfloat new_position_y);
  void set_position_nodpiscale(coordtype const &new_position);
  void set_position_nodpiscale(GLfloat new_position_x, GLfloat new_position_y);
  void set_size(coordtype const &new_size);
  void set_size(GLfloat new_size_x, GLfloat new_size_y);
  void set_size_nodpiscale(coordtype const &new_size);
  void set_size_nodpiscale(GLfloat new_size_x, GLfloat new_size_y);
  void move(  coordtype const &offset);
  void grow(  coordtype const &increase);
  void shrink(coordtype const &decrease);
  void scale( coordtype const &factor);
  void stretch_to_label();
  void stretch_to_label_horizontally();
  void stretch_to_label_vertically();
  void shrink_to_label();
  void shrink_to_label_horizontally();
  void shrink_to_label_vertically();
  void set_colours(colourset const &new_colours);
  void set_colour(        colourtype const &background, colourtype const &outline, colourtype const &content);
  void set_colour_default(colourtype const &background, colourtype const &outline, colourtype const &content);
  void set_colour_hover(  colourtype const &background, colourtype const &outline, colourtype const &content);
  void set_colour_focus(  colourtype const &background, colourtype const &outline, colourtype const &content);
  void set_colour_active( colourtype const &background, colourtype const &outline, colourtype const &content);
  font &get_label_font();
  virtual void set_label(std::string const &newlabel);
protected:
  virtual coordtype const get_absolute_position() const;
public:
  coordtype get_position() const;
  coordtype get_position_nodpiscale() const;
  coordtype get_size() const;
  coordtype get_size_nodpiscale() const;
  bool const &is_visible() const;
  bool const &is_active() const;
  virtual base *get_picked(coordtype const &cursor_position);
  std::string const &get_label();
  template<class T, class ...Args> void add_layout_rule(T thisrule, Args &&...args);

  // updating
  virtual void update();
  virtual void on_press();
  virtual void on_release();
  void centre_to_gui();

  // rendering
  virtual void init_buffer();
  virtual void destroy_buffer();
protected:
  virtual void setup_buffer();
public:
  void arrange_label();
  void update_label_alignment();
protected:
  void setup_label();
public:
  virtual void update_layout();
  virtual void refresh();
  void refresh_position_only();

  virtual void render();
};

template<class T, class ...Args> void base::add_layout_rule(T thisrule, Args &&...args) {
  /// Insertion helper function to build a function object with the right parameters for a layout rule
  // each of these options works, and should be effectively equivalent, but lambdas may be faster, see http://stackoverflow.com/questions/6868171/c0x-lambda-wrappers-vs-bind-for-passing-member-functions
  //layout_rules.emplace_back(std::bind(thisrule, *this, args...));
  //layout_rules.emplace_back(std::bind(thisrule, *this, std::forward<Args>(args)...));
  layout_rules.emplace_back([thisrule, this, args...](){thisrule(*this, args...);});
  // the following references are not safe to use here:
  //layout_rules.emplace_back([thisrule, this, &args...](){thisrule(*this, args...);});
  //layout_rules.emplace_back([&thisrule, this, &args...](){thisrule(*this, std::forward<Args>(args)...);});
}

}

#endif // GUISTORM_BASE_H_INCLUDED
