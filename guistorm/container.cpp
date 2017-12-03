#include "container.h"
#include <boost/range/adaptor/reversed.hpp>
#include "cast_if_required.h"
#include "base.h"
#ifndef NDEBUG
  #include <iostream>
#endif

namespace guistorm {

container::container() {
  /// Default constructor
}

container::~container() {
  /// Default destructor
  clear();
}

unsigned int container::add(base *element) {
  /// Add a new element to this gui and return its index
  #ifndef NDEBUG
    if(!element) {
      std::cout << "ERROR: " << __PRETTY_FUNCTION__ << ": attempted to add a nullptr element" << std::endl;
      return 0;
    }
    if(lock_iterating) {
      std::cout << "GUIStorm: ERROR: attempting to add to a container while iterating through it!  This must never happen!" << std::endl;
      abort();
    }
  #endif
  elements.emplace_back(element);
  elements.back()->parent = this;
  add_to_gui(element);
  return cast_if_required<unsigned int>(elements.size()) - 1;
}

void container::remove(unsigned int index) {
  /// Remove an element from this gui by its index
  #ifndef NDEBUG
    if(index >= elements.size()) {
      std::cout << "ERROR: " << __PRETTY_FUNCTION__ << ": attempted to access element " << index << " outside array bounds " << elements.size() << std::endl;
      return;
    }
    if(lock_iterating) {
      std::cout << "GUIStorm: ERROR: attempting to remove an element from a container while iterating through it!  This must never happen!" << std::endl;
      abort();
    }
  #endif
  elements.erase(elements.begin() + index);
}
void container::remove(base const *const thiselement) {
  /// Remove an element from this gui by its address
  #ifndef NDEBUG
    if(lock_iterating) {
      std::cout << "GUIStorm: ERROR: attempting to remove an element from a container while iterating through it!  This must never happen!" << std::endl;
      abort();
    }
  #endif
  //elements.erase(elements.begin() + index);
  //elements.erase(std::remove_if(elements.begin(), elements.end(), is_purchased_plot), elements.end());
  elements.erase(std::find(elements.begin(), elements.end(), thiselement));
}

base *container::get(unsigned int index) const {
  /// Return the current address of an element from this gui by its index (should not be stored)
  #ifndef NDEBUG
    if(index >= elements.size()) {
      std::cout << "ERROR: " << __PRETTY_FUNCTION__ << ": attempted to access element " << index << " outside array bounds " << elements.size() << ", exiting" << std::endl;
      abort();                                                                  // there's nothing sane we can safely return in this situation
    }
  #endif
  return elements[index];
}

void container::clear() {
  /// Clear the elements container and destroy each element
  #ifndef NDEBUG
    if(lock_iterating) {
      std::cout << "GUIStorm: ERROR: attempting to clear a container while iterating through it!  This must never happen!" << std::endl;
      abort();
    }
  #endif
  for(auto &element : elements) {
    delete element;
  }
  elements.clear();
}

base *container::get_picked(coordtype const &cursor_position) {
  /// Return true if the mouse is over any element in this container
  #ifndef NDEBUG
    lock_iterating = true;
  #endif
  for(auto &element : elements | boost::adaptors::reversed) {                   // we iterate in reverse so most on-top object from equal tiers appears first
    base *picked_element(element->get_picked(cursor_position));
    if(picked_element) {
      #ifndef NDEBUG
        lock_iterating = false;
      #endif
      return picked_element;                                                    // early exit on the first positive result
    }
  }
  #ifndef NDEBUG
    lock_iterating = false;
  #endif
  return nullptr;
}

coordtype const container::get_absolute_position() const {
  /// Return the absolute screen coords of the origin of this element
  /// This function is only called on a container that is not also an element, so it is always going to be top level
  return coordtype(0, 0);
}

void container::destroy_buffer() {
  /// Clean up the buffers in preparation for exit or context switch
  #ifndef NDEBUG
    lock_iterating = true;
  #endif
  for(auto &element : elements) {
    element->destroy_buffer();
  }
  #ifndef NDEBUG
    lock_iterating = false;
  #endif
}

void container::update_layout() {
  /// Reorganise any child elements within this container to the appropriate layout
  #ifndef NDEBUG
    lock_iterating = true;
  #endif
  for(auto &element : elements) {
    element->update_layout();
  }
  #ifndef NDEBUG
    lock_iterating = false;
  #endif
}

void container::refresh() {
  /// Re-create the buffers of all elements in this container
  #ifndef NDEBUG
    lock_iterating = true;
  #endif
  for(auto &element : elements) {
    element->refresh();
  }
  #ifndef NDEBUG
    lock_iterating = false;
  #endif
}

void container::render() {
  /// Recursively render the contents of this container
  #ifndef NDEBUG
    lock_iterating = true;
  #endif
  for(auto &element : elements) {
    element->render();
  }
  #ifndef NDEBUG
    lock_iterating = false;
  #endif
}

}
