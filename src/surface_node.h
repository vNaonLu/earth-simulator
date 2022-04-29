#ifndef __ESIM_SURFACE_NODE_H_
#define __ESIM_SURFACE_NODE_H_

#include "utils.h"

namespace esim {

class surface_node {
public:
  bool is_ready() const noexcept;

  void draw(int width, int height) noexcept;

  ~surface_node() = default;

  surface_node(uint32_t zoom, uint32_t xtile, uint32_t ytile,
               gl_error_callback cb) noexcept;

private:
  class impl;
  impl *pimpl;

};

} // namespace esim

#endif