#ifndef GUISTORM_INPUT_TEXT_H_INCLUDED
#define GUISTORM_INPUT_TEXT_H_INCLUDED

#include "widget.h"
#include <string>
#include <functional>

namespace guistorm {

class input_text : public widget {
private:
  unsigned int cursor = 0;                                                      // cursor position in the label string - which character it's before
  coordtype cursor_position;                                                    // cached cursor rendering position

  GLuint vbo_cursor      = 0;                                                   // vertex buffer for cursor in GL_QUADS format
  GLuint ibo_cursor      = 0;                                                   // index buffer for cursor
  GLuint numverts_cursor = 0;                                                   // number of vertices to render for the cursor

  bool cursor_visible = false;                                                  // whether the cursor should be drawn right now

public:
  input_text(container *parent,
             colourset const &colours,
             std::string const &label = std::string(),
             font *label_font = nullptr,
             coordtype const &size     = coordtype(),
             coordtype const &position = coordtype());
protected:
  virtual ~input_text() override;

public:
  virtual void select_as_input() override final;

  virtual void on_release() override final;

  virtual void init_buffer() override final;
  virtual void destroy_buffer() override final;
protected:
  virtual void setup_buffer() override final;
  virtual void setup_label() override final;
public:
  virtual void render() override final;

  void selected_as_input();
  void deselected_as_input();

  // input: insertion
  void insert(char character);
  void insert(char32_t codepoint);

  // input: cursor position controls
  void cursor_left();
  void cursor_right();
  void cursor_up();
  void cursor_down();
  void cursor_home();
  void cursor_end();
  void cursor_backspace();
  void cursor_delete();

private:
  coordtype get_cursor_position() const;
  void update_cursor();
};

}

#endif // GUISTORM_INPUT_TEXT_H_INCLUDED
