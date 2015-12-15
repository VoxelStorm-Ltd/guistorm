#include "input_text.h"
#ifndef GUISTORM_NO_UTF
  #include "utf8/utf8.h"
#endif // GUISTORM_NO_UTF
#include "cast_if_required.h"
#include "gui.h"

namespace guistorm {

input_text::input_text(container *newparent,
                       colourset const &newcolours,
                       std::string const &newlabel,
                       font *newlabel_font,
                       unsigned int this_length_limit,
                       coordtype const &thissize,
                       coordtype const &thisposition)
  : widget(newparent, newcolours, newlabel, newlabel_font, thissize, thisposition),
    length_limit(this_length_limit) {
  /// Specific constructor
  focusable = true;
  set_length_limit(length_limit);
  cursor_end();                                                                 // wind the cursor to the end for input
}

input_text::~input_text() {
  /// Default destructor
  if(parent_gui->current_input_field == this) {                                 // if this is the currently selected input field while being destroyed
    parent_gui->deselect_input_field();                                         // then make sure it's unselected before we disappear
  }
}

void input_text::select_as_input() {
  #ifdef DEBUG_GUISTORM
    std::cout << "GUIStorm: DEBUG: text input " << get_label() << " requested to select for input" << std::endl;
  #endif // DEBUG_GUISTORM
  parent_gui->select_input_field(this);
}

void input_text::on_release() {
  /// Process anything that happens when this element was being clicked / held and is released
  #ifdef DEBUG_GUISTORM
    std::cout << "GUIStorm: DEBUG: text input " << get_label() << " activating..." << std::endl;
  #endif // DEBUG_GUISTORM
  parent_gui->select_input_field(this);
  #ifdef DEBUG_GUISTORM
    std::cout << "GUIStorm: DEBUG: text input " << get_label() << " finished" << std::endl;
  #endif // DEBUG_GUISTORM
}

void input_text::init_buffer() {
  /// Generate the buffers for this object
  glGenBuffers(1, &vbo_cursor);
  glGenBuffers(1, &ibo_cursor);
  base::init_buffer();
}
void input_text::destroy_buffer() {
  /// Clean up the buffers in preparation for exit or context switch
  glDeleteBuffers(1, &vbo_cursor);
  glDeleteBuffers(1, &ibo_cursor);
  vbo_cursor = 0;
  ibo_cursor = 0;
  numverts_cursor = 0;
  base::destroy_buffer();
}
void input_text::setup_buffer() {
  /// Create or update the buffer for this element
  if(__builtin_expect(vbo_cursor == 0, 0)) {  // if the buffer hasn't been generated yet (unlikely)
    init_buffer();
  }

  std::vector<vertex> vbodata;
  coordtype const corner0(cursor_position);
  coordtype const corner1(cursor_position + Vector2<GLfloat>(2.0f, 10.0f));
  // TODO: scale the cursor appropriately to the text
  unsigned int ibo_offset = cast_if_required<GLuint>(vbodata.size());
  vbodata.reserve(4);
  vbodata.emplace_back(parent_gui->coord_transform(coordtype(corner0.x, corner0.y)));
  vbodata.emplace_back(parent_gui->coord_transform(coordtype(corner1.x, corner0.y)));
  vbodata.emplace_back(parent_gui->coord_transform(coordtype(corner1.x, corner1.y)));
  vbodata.emplace_back(parent_gui->coord_transform(coordtype(corner0.x, corner1.y)));
  std::vector<GLuint> ibodata {
    ibo_offset + 0,
    ibo_offset + 1,
    ibo_offset + 2,
    #ifdef GUISTORM_AVOIDQUADS
      ibo_offset + 0,                                                           // doing this as indexed triangles instead of deprecated quads costs 50% more index entries
      ibo_offset + 2,
    #endif // GUISTORM_AVOIDQUADS
    ibo_offset + 3
  };
  numverts_cursor = cast_if_required<GLuint>(ibodata.size());


  glBindBuffer(GL_ARRAY_BUFFER,         vbo_cursor);
  glBufferData(GL_ARRAY_BUFFER,         vbodata.size() * sizeof(vertex), &vbodata[0], GL_STATIC_DRAW);
  #ifdef GUISTORM_UNBIND
    glBindBuffer(GL_ARRAY_BUFFER,         0);
  #endif // GUISTORM_UNBIND
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cursor);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, numverts_cursor * sizeof(GLuint), &ibodata[0], GL_STATIC_DRAW);
  #ifdef GUISTORM_UNBIND
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  #endif // GUISTORM_UNBIND

  base::setup_buffer();
}

void input_text::setup_label() {
  /// Wrapper around uploading the label that also appends a cursor update
  bool update_required = label_lines.empty();
  base::setup_label();
  if(update_required) {
    update_cursor();
  }
}

void input_text::render() {
  /// Draw the base element with a cursor overlaid if appropriate
  base::render();

  if(!visible || !cursor_visible) {
    return;
  }
  glBindBuffer(GL_ARRAY_BUFFER,         vbo_cursor);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cursor);
  glVertexAttribPointer(parent_gui->attrib_coords,    2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::coords)));
  glVertexAttribPointer(parent_gui->attrib_texcoords, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::texcoords)));

  glUniform4f(parent_gui->uniform_colour,
              colours.current.content.r,
              colours.current.content.g,
              colours.current.content.b,
              colours.current.content.a);
  #ifdef GUISTORM_AVOIDQUADS
    glDrawElements(GL_TRIANGLES, numverts_cursor, GL_UNSIGNED_INT, 0);
  #else
    glDrawElements(GL_QUADS, numverts_cursor, GL_UNSIGNED_INT, 0);
  #endif // GUISTORM_AVOIDQUADS
}

void input_text::selected_as_input() {
  /// Notification function: called when this gets selected as global input
  #ifdef DEBUG_GUISTORM
    std::cout << "GUIStorm: DEBUG: text input " << get_label() << " selected for global input" << std::endl;
  #endif // DEBUG_GUISTORM
  cursor_visible = true;
}
void input_text::deselected_as_input() {
  /// Notification function: called when this gets deselected as global input
  #ifdef DEBUG_GUISTORM
    std::cout << "GUIStorm: DEBUG: text input " << get_label() << " deselected for global input" << std::endl;
  #endif // DEBUG_GUISTORM
  cursor_visible = false;
}

unsigned int input_text::get_length_limit() const {
  return length_limit;
}
void input_text::set_length_limit(unsigned int new_limit) {
  length_limit = new_limit;
  #ifdef GUISTORM_NO_UTF
    size_t const length = label_text.length();
  #else
    #ifdef GUISTORM_UNSAFEUTF
      size_t const length = utf8::unchecked::distance(label_text.begin(), label_text.end());
    #else
      size_t const length = utf8::distance(label_text.begin(), label_text.end());
    #endif // GUISTORM_UNSAFEUTF
  #endif // GUISTORM_NO_UTF
  if(length > length_limit) {                                                   // it's too long, so trim the string to fit inside the limit
    auto it = label_text.begin();
    for(unsigned int i = 0; i != length_limit; ++i) {                           // find the utf8 character at the length limit
      #ifdef GUISTORM_UNSAFEUTF
        utf8::unchecked::next(it);
      #else
        utf8::next(it, label_text.end());
      #endif // GUISTORM_UNSAFEUTF
    }
    label_text.erase(it, label_text.end());                                     // trim off anything remaining after the iterator
  }
}
bool input_text::is_multiline_allowed() const {
  return multiline_allowed;
}
void input_text::set_multiline_allowed(bool new_allowed) {
  if(multiline_allowed == true && new_allowed == false) {
    // multiline was previously allowed and is now disabled, so we need to check for and remove any line breaks
    for(auto it = label_text.begin(); it != label_text.end();) {                // iterate through the string by utf8 chars
      #ifdef GUISTORM_NO_UTF
        char const codepoint = *it;
        ++it;
        if(codepoint == '\n' || codepoint == '\r') {                            // find the first newline
      #else
        #ifdef GUISTORM_UNSAFEUTF
          char32_t const codepoint = utf8::unchecked::next(it);
        #else
          char32_t const codepoint = utf8::next(it, label_text.end());
        #endif // GUISTORM_UNSAFEUTF
        if(codepoint == U'\n' || codepoint == U'\r') {                          // find the first newline
      #endif // GUISTORM_NO_UTF
        label_text.erase(it, label_text.end());                                 // trim off anything remaining after the newline
        break;
      }
    }
  }
  multiline_allowed = new_allowed;
}

void input_text::insert(char character) {
  /// insert the character at the selected position
  if(cursor == label_text.length()) {                                           // are we at the end of the string?
    label_text += character;                                                    // just append to the end
  } else {
    label_text.insert(cursor, &character, 1);                                   // insert mid-string (buffer insert mode)
  }
  ++cursor;                                                                     // advance the cursor, no need to worry about unicode
  refresh();                                                                    // we've altered the label text so refresh it
}
#ifndef GUISTORM_NO_UTF
void input_text::insert(char32_t codepoint) {
  /// insert the UTF32 codepoint at the selected position
  if(label_text.length() == length_limit) {
    #ifdef DEBUG_GUISTORM
      std::cout << "GUIStorm: DEBUG: text input " << get_label() << " reached its length limit of " << length_limit << std::endl;
    #endif // DEBUG_GUISTORM
    return;                                                                     // can't enter any more text, we're at length limit
  }
  if(cursor == label_text.length()) {                                           // are we at the end of the string?
    #ifdef GUISTORM_UNSAFEUTF
      utf8::unchecked::append(codepoint, std::back_inserter(label_text));       // just append to the end
    #else
      utf8::append(codepoint, std::back_inserter(label_text));                  // just append to the end
    #endif // GUISTORM_UNSAFEUTF
  } else {
    #ifdef GUISTORM_UNSAFEUTF
      utf8::unchecked::append(codepoint, std::inserter(label_text, label_text.begin() + cursor)); // insert mid-string
    #else
      utf8::append(codepoint, std::inserter(label_text, label_text.begin() + cursor)); // insert mid-string
    #endif // GUISTORM_UNSAFEUTF
  }
  auto it = label_text.begin() + cursor;
  #ifdef GUISTORM_UNSAFEUTF
    utf8::unchecked::next(it);                                                  // advance the cursor
  #else
    utf8::next(it, label_text.end());
  #endif // GUISTORM_UNSAFEUTF
  cursor = cast_if_required<unsigned int>(it - label_text.begin());
  refresh();                                                                    // we've altered the label text so refresh it
}
#endif // GUISTORM_NO_UTF
void input_text::cursor_left() {
  if(cursor == 0) {
    return;
  }
  #ifdef GUISTORM_NO_UTF
    --cursor;
  #else
    auto it = label_text.begin() + cursor;
    #ifdef GUISTORM_UNSAFEUTF
      utf8::unchecked::prior(it);                                               // shift the cursor backwards
    #else
      utf8::prior(it, label_text.begin());
    #endif // GUISTORM_UNSAFEUTF
    cursor = cast_if_required<unsigned int>(it - label_text.begin());
  #endif // GUISTORM_NO_UTF
  update_cursor();
}
void input_text::cursor_right() {
  if(cursor == label_text.length()) {
    return;
  }
  #ifdef GUISTORM_NO_UTF
    ++cursor;
  #else
    auto it = label_text.begin() + cursor;
    #ifdef GUISTORM_UNSAFEUTF
      utf8::unchecked::next(it);                                                // advance the cursor
    #else
      utf8::next(it, label_text.end());
    #endif // GUISTORM_UNSAFEUTF
    cursor = cast_if_required<unsigned int>(it - label_text.begin());
  #endif // GUISTORM_NO_UTF
  update_cursor();
}
void input_text::cursor_up() {
  // TODO
  update_cursor();
}
void input_text::cursor_down() {
  // TODO
  update_cursor();
}
void input_text::cursor_home() {
  cursor = 0;
  update_cursor();
}
void input_text::cursor_end() {
  cursor = cast_if_required<unsigned int>(label_text.length());
  update_cursor();
}
void input_text::cursor_backspace() {
  /// Delete the character before the cursor
  if(cursor == 0) {
    return;
  }
  unsigned int const cursor_last = cursor;
  #ifdef GUISTORM_NO_UTF
    --cursor;
  #else
    auto it = label_text.begin() + cursor;
    #ifdef GUISTORM_UNSAFEUTF
      utf8::unchecked::prior(it);                                               // shift the cursor backwards
    #else
      utf8::prior(it, label_text.begin());
    #endif // GUISTORM_UNSAFEUTF
    cursor = cast_if_required<unsigned int>(it - label_text.begin());
  #endif // GUISTORM_NO_UTF
  label_text.erase(cursor, cursor_last - cursor);                               // erase that character, however wide it may have been
  refresh();                                                                    // we've altered the label text so refresh it
}
void input_text::cursor_delete() {
  /// Delete the character after the cursor
  if(cursor == label_text.length()) {
    return;
  }
  #ifdef GUISTORM_NO_UTF
    unsigned int const cursor_last = cursor + 1;                                // don't move the actual cursor position though
  #else
    auto it_last = label_text.begin() + cursor;
    #ifdef GUISTORM_UNSAFEUTF
      utf8::unchecked::next(it_last);                                           // advance the temporary cursor
    #else
      utf8::next(it_last, label_text.end());
    #endif // GUISTORM_UNSAFEUTF
    unsigned int const cursor_last = cast_if_required<unsigned int>(it_last - label_text.begin());  // don't move the actual cursor position though
  #endif // GUISTORM_NO_UTF
  label_text.erase(cursor, cursor_last - cursor);                               // erase that character, however wide it may have been
  refresh();                                                                    // we've altered the label text so refresh it
}

coordtype input_text::get_cursor_position() const{
  /// Fetch the cursor's visible coordinates
  coordtype pen = label_origin;
  if(cursor == 0) {
    return pen;                                                                 // cursor's at the origin if it's at string start
  }
  unsigned int char_position = 0;
  #ifdef GUISTORM_NO_UTF
    char charcode_last = '\0';
  #else
    char32_t charcode_last = U'\0';
  #endif // GUISTORM_NO_UTF
  for(auto const &thisline : label_lines) {
    for(auto const &thisword : thisline.words) {
      /*
      if(char_position + thisword.glyphs.size() < cursor) {
        char_position += thisword.glyphs.size();
        continue;                                                               // skip the entire word
      }
      */
      for(auto const &thisglyph : thisword.glyphs) {
        ++char_position;
        pen.x += thisglyph->get_kerning(charcode_last);
        charcode_last = thisglyph->charcode;
        pen += thisglyph->advance;
        if(cursor == char_position) {
          return pen;
        }
      }
      pen.x += thisline.spacing;                                                // justification inter-word space expansion
    }
    pen.x = label_origin.x;                                                     // carriage return
    pen.y -= label_line_spacing;                                                // line feed
  }
  return pen;
}
void input_text::update_cursor() {
  /// Update the visible cursor position
  cursor_position = get_cursor_position();
  #ifdef DEBUG_GUISTORM
    std::cout << "GUIStorm: DEBUG: updating cursor on string " << label_text << " position " << cursor << " of " << label_text.length() << " at " << cursor_position << std::endl;
  #endif // DEBUG_GUISTORM
  setup_buffer();
}

}
