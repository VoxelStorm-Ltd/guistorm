public:
  virtual void select_as_input() override final;

  virtual void on_release() override final;

#ifndef GUISTORM_INPUT_TEXT_H_INCLUDED
  virtual void init_buffer() override final;
#define GUISTORM_INPUT_TEXT_H_INCLUDED
  virtual void destroy_buffer() override final;

protected:
#ifndef GUISTORM_NO_TEXT
  virtual void setup_buffer() override final;

  virtual void setup_label() override final;
#include "widget.h"
public:
#include <string>
  virtual void render() override final;
#include <functional>


  // notification
namespace guistorm {
  void selected_as_input();

  void deselected_as_input();

class input_text : public widget {
  // input: settings
private:
  unsigned int get_length_limit() const __attribute__((__pure__));
  unsigned int cursor = 0;                                                      // cursor position in the label string - which character it's before
  void set_length_limit(unsigned int new_limit);
  coordtype cursor_position;                                                    // cached cursor rendering position
  bool is_multiline_allowed() const __attribute__((__pure__));

  void set_multiline_allowed(bool allowed);
  GLuint vbo_cursor      = 0;                                                   // vertex buffer for cursor in GL_QUADS format

  GLuint ibo_cursor      = 0;                                                   // index buffer for cursor
  // input: insertion
  GLuint numverts_cursor = 0;                                                   // number of vertices to render for the cursor
  void insert(char character);

  void insert(char32_t codepoint);
  unsigned int length_limit = 128;                                              // length limit for input

  bool multiline_allowed = false;                                               // whether to allow multiple line input
  // input: cursor position controls

  void cursor_left();
  void cursor_right();
  bool cursor_visible = false;                                                  // whether the cursor should be drawn right now
  void cursor_up();

  void cursor_down();
public:
  void cursor_home();
  input_text(container *parent,
  void cursor_end();
             colourset const &colours,
  void cursor_backspace();
             std::string const &label = "",
  void cursor_delete();
             font *label_font = nullptr,

             unsigned int this_length_limit = 128,
private:
             coordtype const &size     = coordtype(),
  coordtype get_cursor_position() const;
             coordtype const &position = coordtype());
  void update_cursor();
protected:
};
  virtual ~input_text() override;


}

public:
#endif // GUISTORM_NO_TEXT
  virtual void select_as_input() override final;


#endif // GUISTORM_INPUT_TEXT_H_INCLUDED
  virtual void on_release() override final;

  virtual void init_buffer() override final;
  virtual void destroy_buffer() override final;
protected:
  virtual void setup_buffer() override final;
  virtual void setup_label() override final;
public:
  virtual void render() override final;

  // notification
  void selected_as_input();
  void deselected_as_input();

  // input: settings
  unsigned int get_length_limit() const __attribute__((__pure__));
  void set_length_limit(unsigned int new_limit);
  bool is_multiline_allowed() const __attribute__((__pure__));
  void set_multiline_allowed(bool allowed);

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

#endif // GUISTORM_NO_TEXT

#endif // GUISTORM_INPUT_TEXT_H_INCLUDED
