#ifndef __ESIM_CORE_BITMAP_H_
#define __ESIM_CORE_BITMAP_H_

#include "core/utils.h"
#include <string_view>

namespace esim {

namespace core {

class bitmap {
public:
  int width() const noexcept;

  int height() const noexcept;

  const char *buffer() const noexcept;

  bool load_from_file(std::string_view file) noexcept;

  bool load(const char *buffer, size_t size) noexcept;

  bitmap() noexcept;

  ~bitmap() noexcept;

private:
  class opaque;
  uptr<opaque> opaque_;
};

} // namespace core

} // namespace esim

#endif