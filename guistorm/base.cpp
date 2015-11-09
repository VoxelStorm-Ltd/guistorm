#include "base.h"
#include "cast_if_required.h"
#include "gui.h"

namespace guistorm {

base::base(container *newparent,
           colourset const &newcolours,
           std::string const &thislabel,
           font *label_font,
           coordtype const &thissize,
           coordtype const &thisposition)
  : parent(newparent),
    position(thisposition),
    size(thissize),
    colours(newcolours),
    label_text(thislabel),
    label_font(label_font) {
  /// Specific constructor
  #ifndef NDEBUG
    if(!parent) {
      std::cout << "GUIStorm: ERROR: " << __PRETTY_FUNCTION__ << " must not be initialised with no parent" << std::endl;
      return;
    }
  #endif
  parent->add(this);                          // this also assigns the parent_gui pointer
  #ifndef NDEBUG
    if(!parent_gui) {                         // but check this for safety
      std::cout << "GUIStorm: ERROR: " << __PRETTY_FUNCTION__ << " parent chain failed to assign parent_gui, make sure the top level container's parent is a guistorm::gui object" << std::endl;
      return;
    }
  #endif
  position *= parent_gui->get_dpi_scale();    // these must come after parent gui assignment
  size     *= parent_gui->get_dpi_scale();
}

base::~base() {
  /// Default destructor
  destroy_buffer();
}

void base::operator delete(void *p __attribute__((__unused__))) {
  /// guistorm garbage-collects all owned entities, so warn the user on attempting to delete directly
  std::cout << "GUIStorm: WARNING: GUIStorm owned objects should not be deleted manually!" << std::endl;
}

void base::show() {
  /// Make this element visible for rendering
  visible = true;
}
void base::hide() {
  /// Do not render this element
  visible = false;
}
void base::toggle() {
  /// Flip the rendering state of this element
  visible = !visible;
}
void base::set_position(coordtype const &new_position) {
  /// Update this element's relative position to its parent element or the screen centre if parentless, lower left corner
  position = new_position * parent_gui->get_dpi_scale();
  refresh_position_only();
}
void base::set_position(GLfloat new_position_x, GLfloat new_position_y) {
  set_position(coordtype(new_position_x, new_position_y));    // wrapper
}
void base::set_position_nodpiscale(coordtype const &new_position) {
  /// Update this element's relative position to its parent element or the screen centre if parentless, lower left corner - not scaled by dpi
  position = new_position;
  refresh_position_only();
}
void base::set_position_nodpiscale(GLfloat new_position_x, GLfloat new_position_y) {
  set_position_nodpiscale(coordtype(new_position_x, new_position_y));    // wrapper
}
void base::set_size(coordtype const &new_size) {
  /// Update this element's size
  size = new_size * parent_gui->get_dpi_scale();
  refresh_position_only();
}
void base::set_size(GLfloat new_size_x, GLfloat new_size_y) {
  set_size(coordtype(new_size_x, new_size_y));                // wrapper
}
void base::set_size_nodpiscale(coordtype const &new_size) {
  /// Update this element's size - not scaled by dpi
  size = new_size;
  refresh_position_only();
}
void base::set_size_nodpiscale(GLfloat new_size_x, GLfloat new_size_y) {
  set_size_nodpiscale(coordtype(new_size_x, new_size_y));                // wrapper
}
void base::move(coordtype const &offset) {
  /// Move this element relative to its existing position
  position += offset * parent_gui->get_dpi_scale();
  refresh_position_only();
}
void base::grow(coordtype const &increase) {
  /// Scale this element up by a specified increase
  size += increase * parent_gui->get_dpi_scale();
  refresh_position_only();
}
void base::shrink(coordtype const &decrease) {
  /// Scale this element down by a specified decrease
  size -= decrease * parent_gui->get_dpi_scale();
  refresh_position_only();
}
void base::scale(coordtype const &factor) {
  /// Scale this element by a specified factor in each direction
  size *= factor;
  refresh_position_only();
}

void base::stretch_to_label() {
  /// Expand the size of this object to encompass its label contents plus margin
  stretch_to_label_horizontally();
  stretch_to_label_vertically();
}
void base::stretch_to_label_horizontally() {
  /// Expand the width of this object to encompass its label contents plus margin
  // setup_label must have been run for this to operate
  #ifndef NDEBUG
    if(size.x == 0.0f) {
      std::cout << "GUIStorm: WARNING: " << __PRETTY_FUNCTION__ << " attempted to operate with zero width label; has setup_label been called yet?" << std::endl;
      return;
    }
  #endif
  if(label_size.x + (label_margin.x * 2) > size.x) {
    size.x = label_size.x + (label_margin.x * 2);
    refresh_position_only();
  }
}
void base::stretch_to_label_vertically() {
  /// Expand the height of this object to encompass its label contents plus margin
  GLfloat const targetsize = label_size.y + (label_margin.y * 2) + get_label_font().metrics_height;
  if(targetsize > size.y) {
    size.y = targetsize;
    refresh_position_only();
  }
}
void base::shrink_to_label() {
  /// Shrink the size of this object so it is no larger than its label contents plus margin
  shrink_to_label_horizontally();
  shrink_to_label_vertically();
}
void base::shrink_to_label_horizontally() {
  /// Shrink the width of this object so it is no larger than the widest point of its label contents plus margin
  GLfloat const targetsize = label_size.x + (label_margin.x * 2);
  if(targetsize < size.x) {
    size.x = targetsize;
    refresh_position_only();
  }
}
void base::shrink_to_label_vertically() {
  /// Shrink the height of this object so it is no larger than the height point of its label contents plus margin
  GLfloat const targetsize = label_size.y + (label_margin.y * 2) + get_label_font().metrics_height;
  if(targetsize < size.y) {
    size.y = targetsize;
    refresh_position_only();
  }
}

void base::set_colours(colourset const &new_colours) {
  /// Update the full set of current colours from an existing colourset
  colours = new_colours;
}
void base::set_colour(colourtype const &background, colourtype const &outline, colourtype const &content) {
  /// Update the current (momentary) colour - use this for initial colours, fade-ins and flashing effects
  colours.current.assign(background, outline, content);
}
void base::set_colour_default(colourtype const &background, colourtype const &outline, colourtype const &content) {
  /// Update the default colour used when this element is idle
  colours.idle.assign(background, outline, content);
}
void base::set_colour_hover(colourtype const &background, colourtype const &outline, colourtype const &content) {
  /// Update the colour to fade to when moused over
  colours.hover.assign(background, outline, content);
}
void base::set_colour_focus(colourtype const &background, colourtype const &outline, colourtype const &content) {
  /// Update the colour to fade to when focused, i.e. with keyboard selection
  colours.focus.assign(background, outline, content);
}
void base::set_colour_active(colourtype const &background, colourtype const &outline, colourtype const &content) {
  /// Update the colour to fade to when activated, i.e. clicked or dragged or text has been entered
  colours.active.assign(background, outline, content);
}

font &base::get_label_font() {
  /// Safely return a reference to the font we use for this object, whether it's
  /// its own specific label font or the gui's default or another fallback
  /// Note - this cannot fail to return a valid font, otherwise it exits fatally
  font *thisfont = label_font;
  if(!thisfont) {                                     // check our font is valid
    thisfont = parent_gui->font_default;              // otherwise try to use the default font
    if(!thisfont) {
      std::cout << "GUIStorm: WARNING: No default font set, and " << __PRETTY_FUNCTION__ << " is trying to query it." << std::endl;
      if(!parent_gui->fonts.empty()) {
        thisfont = *(parent_gui->fonts.begin());      // no default available, so select the first one from the available list
        std::cout << "GUIStorm: WARNING: Defaulting to first entry: " << thisfont->name << " size " << thisfont->font_size << std::endl;
      }
      if(!thisfont) {
        std::cout << "GUIStorm: ERROR: No font available for " << __PRETTY_FUNCTION__ << std::endl;
        abort();                                        // bail out!
      }
    }
  }
  if(!parent_gui->font_atlas) {
    #ifdef DEBUG_GUISTORM
      std::cout << "GUIStorm: DEBUG: parent_gui->font_atlas not yet loaded when arranging label" << std::endl;
    #endif
    parent_gui->load_fonts();
  }
  thisfont->load_if_needed(parent_gui->font_atlas);
  #ifdef DEBUG_GUISTORM
    //std::cout << "GUIStorm: DEBUG: parent_gui->font_atlas->id() = " << parent_gui->font_atlas->id() << std::endl;
  #endif
  //font_atlas_id = parent_gui->font_atlas->id();       // cache the font atlas ID for this font
  return *thisfont;
}

void base::set_label(std::string const &newlabel) {
  if(newlabel == label_text) {
    return;                   // skip updating if we're making no changes
  }
  label_text = newlabel;
  refresh();                  // refresh the buffer (this also clears label lines)
}

coordtype const base::get_absolute_position() const {
  /// Return the absolute internal coords of the origin of this element (not converted to screen dpi)
  coordtype parent_origin(0, 0);
  base *parent_base(dynamic_cast<base*>(parent));
  if(parent_base) {
    return position + parent_base->get_absolute_position();       // obtain its parent position recursively
  } else {
    return position;                                              // its parent is not a base type so has no position of its own
  }
}
coordtype base::get_position() const {
  /// Return the relative position
  return position / parent_gui->get_dpi_scale();
}
coordtype base::get_position_nodpiscale() const {
  /// Return the relative position without dpi scaling
  return position;
}
coordtype base::get_size() const {
  /// Return the size
  return size / parent_gui->get_dpi_scale();
}
coordtype base::get_size_nodpiscale() const {
  /// Return the size without dpi scaling
  return size;
}
bool const &base::is_visible() const {
  /// Return whether it's currently visible
  return visible;
}
bool const &base::is_active() const {
  /// Return whether it's currently active
  return active;
}

base *base::get_picked(coordtype const &cursor_position) {
  /// Return true if the mouse is over this element
  if(!visible) {
    return nullptr;
  }
  coordtype const &position_absolute(get_absolute_position());
  if(cursor_position.x >= position_absolute.x &&
     cursor_position.y >= position_absolute.y &&
     cursor_position.x <= position_absolute.x + size.x &&
     cursor_position.y <= position_absolute.y + size.y) {
    return this;
  } else {
    mouseover = false;
    return nullptr;
  }
}

std::string const &base::get_label() {
  return label_text;
}

void base::update() {
  /// Update colours based on current state
  // check if we're mousing over this
  mouseover = false;
  active    = false;
  if(focusable) {
    if(this == parent_gui->picked_element) {
      mouseover = true;
      // check if we're clicking on it
      if(parent_gui->mouse_pressed) {
        active = true;
        on_press();
        colours.blend_to_active(0.5);
      } else {
        if(parent_gui->mouse_released) {
          on_release();
        }
        colours.blend_to_hover(0.5);
      }
    } else {
      if(focused) {
        colours.blend_to_focus(0.1);
      } else {
        colours.blend_to_idle(0.05);
      }
    }
  } else {
    colours.blend_to_idle(0.05);
  }
}

void base::on_press() {
  /// Process anything that happens when this element is being clicked / held down
  // default is noop
}

void base::on_release() {
  /// Process anything that happens when this element was being clicked / held and is released
  // default is noop
}

void base::centre_to_gui() {
  /// Helper function to reposition this object so it's centered in the parent GUI
  set_position((parent_gui->windowsize / 2.0) - (get_size() / 2.0));
}

void base::init_buffer() {
  /// Generate the buffers for this object
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ibo);
  glGenBuffers(1, &vbo_label);
  glGenBuffers(1, &ibo_label);
}
void base::destroy_buffer() {
  /// Clean up the buffers in preparation for exit or context switch
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ibo);
  glDeleteBuffers(1, &vbo_label);
  glDeleteBuffers(1, &ibo_label);
  vbo = 0;
  ibo = 0;
  vbo_label = 0;
  ibo_label = 0;
  numverts = 0;
  numverts_label = 0;
  initialised = false;
}
void base::setup_buffer() {
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
  std::vector<GLuint> ibodata{0, 1, 2, 3};
  numverts = 4;

  glBindBuffer(GL_ARRAY_BUFFER,         vbo);
  glBufferData(GL_ARRAY_BUFFER,         vbodata.size() * sizeof(vertex), &vbodata[0], GL_STATIC_DRAW);
  //glBindBuffer(GL_ARRAY_BUFFER,         0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, numverts       * sizeof(GLuint), &ibodata[0], GL_STATIC_DRAW);
  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  setup_label();                          // set up the label buffer

  initialised = true;
}

void base::arrange_label() {
  /// Called by setup_label, but can be called manually to just update text.
  font &this_label_font(get_label_font());
  // compose the text layout in the abstract first
  label_line_spacing = this_label_font.metrics_height;
  label_lines.clear();
  label_lines.emplace_back();                           // create a default first line

  std::vector<font::word> words;
  words.emplace_back();
  for(size_t i = 0; i != label_text.length(); ++i) {
    font::glyph const *tempglyph = this_label_font.getglyph(label_text[i]);
    if(!tempglyph) {
      std::cout << "GUIStorm: WARNING: Requested unmapped character \"" << label_text[i] << "\" (ascii " << static_cast<int>(label_text[i]) << ")" << std::endl;
      tempglyph = this_label_font.getglyph(' ');       // replace unknown characters with space
    }
    //if(tempglyph.advance.y > label_line_spacing) {
    //  label_line_spacing = tempglyph.advance.y;         // update uniform minimum line spacing
    //}
    bool wordbreak_here = false;
    bool printchar_here = true;
    if(tempglyph->linebreak) {                          // newline or carriage return
      words.back().linebreak = true;
      //words.back().advance.x = 0.0f;
      if(!label_merge_newlines) {
        wordbreak_here = true;                          // add newlines if we aren't merging them
      }
      printchar_here = false;
    } else {
      if(i != 0) {                                      // don't check for word breaks if this is the first character
        if(!words.back().glyphs.empty() &&              // if the last word has any characters...
           words.back().glyphs.back()->is_blank) {      // ...and the last character was invisible...
          if(!tempglyph->is_blank ||                    // ...and this one is visible,
             !label_merge_whitespace) {                 // ...or we aren't merging whitespace, then
            wordbreak_here = true;                      // ...start a new word
          }
        }
      }
    }
    if(wordbreak_here) {
      #ifdef DEBUG_GUISTORM
        //words.back().glyphs.emplace_back(this_label_font.getglyph(']'));
      #endif
      words.emplace_back();
      #ifdef DEBUG_GUISTORM
        //words.back().glyphs.emplace_back(this_label_font.getglyph('['));
      #endif
    }
    if(printchar_here) {
      words.back().glyphs.emplace_back(tempglyph);
    } else {
      #ifdef DEBUG_GUISTORM
        //words.back().glyphs.emplace_back(this_label_font.getglyph('/'));
      #endif
    }
  }

  // carry out word-wrapping
  label_size.assign(0.0f, label_line_spacing * -0.5f);  // start the height with 1 line thickess minimum
  coordtype const text_area(size - (label_margin * 2));
  GLfloat hpos = 0.0f;
  for(auto const &thisword : words) {
    GLfloat wordlength = thisword.length();
    hpos += wordlength;
    if(label_wordwrap) {
      if(hpos > text_area.x) {                          // this word would break out of the permissible box area, so start a new line
        if(hpos > label_size.x) {
          label_size.x = hpos -  wordlength;            // this line is the longest (for centering calculations)
        }
        label_lines.emplace_back();                     // line feed
        hpos = wordlength;                              // because we're pushing the wrapped word onto the new line our cursor starts after it
        label_size.y += label_line_spacing;
      }
    }
    label_lines.back().words.emplace_back(thisword);
    if(thisword.linebreak) {
      if(hpos > label_size.x) {
        label_size.x = hpos;                            // this line is the longest (for centering calculations)
      }
      label_lines.back().linebreak = true;
      label_lines.emplace_back();                       // line feed
      hpos = 0.0f;                                      // carriage return
      label_size.y += label_line_spacing;
    }
  }
  if(hpos > label_size.x) {
    label_size.x = hpos;                                // this line is the longest (for centering calculations)
  }
  #ifdef DEBUG_GUISTORM
    //std::cout << "GUIStorm: DEBUG: words wrapped to " << label_lines.size() << " lines max size " << label_size << std::endl;
  #endif // DEBUG_GUISTORM

  // calculate and cache the max line length of each line and the max overall
  for(auto &thisline : label_lines) {
    thisline.size.x = thisline.length();                // relatively expensive calculation, so we cache the result
    if(thisline.size.x > label_size.x) {
      label_size.x = thisline.size.x;
    }
  }

  // carry out justification if required
  if(label_justify_horizontal && label_lines.size() > 1) {              // don't justify anything consisting of one line (or none)
    for(auto &thisline : boost::make_iterator_range(label_lines.begin(), --label_lines.end())) {    // don't justify the last line
      if(!thisline.linebreak && thisline.words.size() > 1) {            // don't try to justify one-word lines or lines that are intentionally split
        thisline.spacing = (label_size.x - thisline.size.x) / static_cast<float>(thisline.words.size() - 1);
      }
      #ifdef DEBUG_GUISTORM
        //std::cout << "GUIStorm: DEBUG: line justification spacing " << thisline.spacing << std::endl;
      #endif // DEBUG_GUISTORM
    }
  }
  if(label_stretch_vertical) {
    stretch_to_label_vertically();
  }
  if(label_shrink_vertical) {
    shrink_to_label_vertically();
  }
}

void base::update_label_alignment() {
  /// decide on label positioning and reshuffle the layout for justifications
  coordtype const label_position(get_absolute_position());
  switch(label_alignment) {       // horizontal
  case aligntype::CENTRE:
  case aligntype::TOP:
  case aligntype::BOTTOM:
    label_origin.x = label_position.x + ((size.x - label_size.x) / 2.0f);     // the margins simplify out
    break;
  case aligntype::LEFT:
  case aligntype::TOP_LEFT:
  case aligntype::BOTTOM_LEFT:
    label_origin.x = label_position.x + (label_margin.x * parent_gui->dpi_scale);
    break;
  case aligntype::RIGHT:
  case aligntype::TOP_RIGHT:
  case aligntype::BOTTOM_RIGHT:
    label_origin.x = label_position.x - (label_margin.x * parent_gui->dpi_scale) + size.x - label_size.x;
    break;
  }
  switch(label_alignment) {       // vertical
  case aligntype::CENTRE:
  case aligntype::LEFT:
  case aligntype::RIGHT:
    label_origin.y = label_position.y + ((size.y + label_size.y) / 2.0f);     // the margins simplify out
    break;
  case aligntype::TOP:
  case aligntype::TOP_LEFT:
  case aligntype::TOP_RIGHT:
    label_origin.y = label_position.y - (label_margin.y * parent_gui->dpi_scale) + size.y - get_label_font().metrics_height;
    break;
  case aligntype::BOTTOM:
  case aligntype::BOTTOM_LEFT:
  case aligntype::BOTTOM_RIGHT:
    label_origin.y = label_position.y + (label_margin.y * parent_gui->dpi_scale);
    break;
  }
}

void base::setup_label() {
  /// Upload just the label portion of the buffer
  if(label_lines.empty()) {
    arrange_label();                                    // only rearrange label if it hasn't already been laid out as this does not require GL context
  }
  update_label_alignment();                             // update position in all cases

  // compose the VBO from the text positioning
  std::vector<vertex> vbodata;
  std::vector<GLuint> ibodata;
  coordtype pen = label_origin;
  char charcode_last = '\0';
  for(auto const &thisline : label_lines) {
    for(auto const &thisword : thisline.words) {
      for(auto const &thisglyph : thisword.glyphs) {
        pen.x += thisglyph->get_kerning(charcode_last);
        charcode_last = thisglyph->charcode;
        if(!thisglyph->is_blank) {                      // whitespace glyphs don't get added but still take up horizontal space
          coordtype const corner0(pen + thisglyph->offset);
          coordtype const corner1(corner0 + thisglyph->size);
          unsigned int ibo_offset = cast_if_required<GLuint>(vbodata.size());
          vbodata.emplace_back(parent_gui->coord_transform(coordtype(corner0.x, corner0.y)), coordtype(thisglyph->texcoord0.x, thisglyph->texcoord0.y));
          vbodata.emplace_back(parent_gui->coord_transform(coordtype(corner1.x, corner0.y)), coordtype(thisglyph->texcoord1.x, thisglyph->texcoord0.y));
          vbodata.emplace_back(parent_gui->coord_transform(coordtype(corner1.x, corner1.y)), coordtype(thisglyph->texcoord1.x, thisglyph->texcoord1.y));
          vbodata.emplace_back(parent_gui->coord_transform(coordtype(corner0.x, corner1.y)), coordtype(thisglyph->texcoord0.x, thisglyph->texcoord1.y));
          ibodata.emplace_back(ibo_offset + 0);
          ibodata.emplace_back(ibo_offset + 1);
          ibodata.emplace_back(ibo_offset + 2);
          #ifdef GUISTORM_AVOIDQUADS
            ibodata.emplace_back(ibo_offset + 0);       // doing this as indexed triangles instead of deprecated quads costs 50% more index entries
            ibodata.emplace_back(ibo_offset + 2);
          #endif
          ibodata.emplace_back(ibo_offset + 3);
        }
        pen += thisglyph->advance;
      }
      pen.x += thisline.spacing;                        // justification inter-word space expansion
    }
    pen.x = label_origin.x;                             // carriage return
    pen.y -= label_line_spacing;                        // line feed
  }
  numverts_label = cast_if_required<GLuint>(ibodata.size());

  #ifdef DEBUG_GUISTORM
    /*
    std::cout << "GUIStorm: Uploading " << vbodata.size() << " " << sizeof(vertex) << "B verts, " << numverts_label << " indices to vbo ("
              << (vbodata.size() * sizeof(vertex)) << "B, "
              << (numverts_label * sizeof(GLuint)) << "B) "
              << "atlas " << parent_gui->font_atlas->id() << std::endl;
    */
  #endif // DEBUG_GUISTORM

  glBindBuffer(GL_ARRAY_BUFFER,         vbo_label);
  glBufferData(GL_ARRAY_BUFFER,         vbodata.size() * sizeof(vertex), &vbodata[0], GL_STATIC_DRAW);
  //glBindBuffer(GL_ARRAY_BUFFER,         0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_label);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, numverts_label * sizeof(GLuint), &ibodata[0], GL_STATIC_DRAW);
  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void base::update_layout() {
  /// Reposition this object in accordance with any layout rules given to it
  //std::cout << "GUIStorm: DEBUG: updating layout for " << get_label() << " with " << layout_rules.size() << " rules..." << std::endl;
  for(auto const &thisrule : layout_rules) {
    thisrule();
  }
  if(!layout_rules.empty()) {
    refresh_position_only();
  }
}

void base::refresh() {
  /// Refresh this object's visual state
  label_lines.clear();        // ensure the label buffer arrangement also gets refreshed
  refresh_position_only();    // refresh the outline shape
}

void base::refresh_position_only() {
  /// Refresh this object's position and size only, don't refresh text content
  initialised = false;
}

void base::render() {
  /// Draw this element
  if(!visible) {
    return;
  }
  if(__builtin_expect(!initialised, 0)) {  // if the buffer hasn't been initialised yet (unlikely)
    setup_buffer();
  }
  if(numverts != 0) {
    glBindBuffer(GL_ARRAY_BUFFER,         vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glVertexAttribPointer(parent_gui->attrib_coords,    2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::coords)));
    glVertexAttribPointer(parent_gui->attrib_texcoords, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::texcoords)));

    if(colours.current.background.a != 0.0f) {                          // skip drawing fully transparent parts
      glUniform4f(parent_gui->uniform_colour,
                  colours.current.background.r,
                  colours.current.background.g,
                  colours.current.background.b,
                  colours.current.background.a);
      glDrawElements(GL_TRIANGLE_FAN, numverts, GL_UNSIGNED_INT, 0);    // background
    }
    if(colours.current.outline.a != 0.0f) {                             // skip drawing fully transparent parts
      glUniform4f(parent_gui->uniform_colour,
                  colours.current.outline.r,
                  colours.current.outline.g,
                  colours.current.outline.b,
                  colours.current.outline.a);
      glDrawElements(GL_LINE_LOOP,    numverts, GL_UNSIGNED_INT, 0);    // outline
    }
  }
  // draw the label
  if(numverts_label != 0) {
    glBindBuffer(GL_ARRAY_BUFFER,         vbo_label);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_label);
    glVertexAttribPointer(parent_gui->attrib_coords,    2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::coords)));
    glVertexAttribPointer(parent_gui->attrib_texcoords, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::texcoords)));

    glUniform4f(parent_gui->uniform_colour,
                colours.current.content.r,
                colours.current.content.g,
                colours.current.content.b,
                colours.current.content.a);
    #ifdef GUISTORM_AVOIDQUADS
      glDrawElements(GL_TRIANGLES, numverts_label, GL_UNSIGNED_INT, 0);
    #else
      glDrawElements(GL_QUADS, numverts_label, GL_UNSIGNED_INT, 0);
    #endif
  }

  update();
}

}
