#ifndef __ES_SURFACE_H_
#define __ES_SURFACE_H_

#include <memory>

namespace esim {

class surface {
public:
  bool draw(int width, int height) noexcept;

  bool is_ready() noexcept;

  surface() noexcept;

  ~surface() noexcept;
  
private:
  class impl;
  std::unique_ptr<impl> pimpl;
};

} // namespace esim

#endif