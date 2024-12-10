#pragma once

#include <vector>
#include <atomic>
#include "types.h"

namespace guistorm {

class base;                                                                     // forward declaration

class container {
  /// virtual class for containing other elements - any element that can contain
  /// others should inherit from this
public:
  std::vector<base*> elements;                                                  // container for all child elements
private:
  #ifndef NDEBUG
    std::atomic_bool lock_iterating{false};                                     // safety interlock in to catch any attempts to modify the container while iterating through it
  #endif
protected:
  container();
  virtual ~container();

public:
  unsigned int add(base *element);
  void remove(unsigned int index);
  void remove(base const *const thiselement);
  #ifdef NDEBUG
    base *get(unsigned int index) const __attribute__((__pure__));
  #else
    base *get(unsigned int index) const;
  #endif // NDEBUG
  void clear();

  virtual void add_to_gui(base *element) = 0;

  virtual base *get_picked(coordtype const &cursor_position);
  virtual coordtype const get_absolute_position() const;

  virtual void destroy_buffer();

  virtual void update_layout();
  virtual void refresh();

  virtual void render();
};

}
