#include "core/bitmap.h"
#include <cassert>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace esim {

namespace core {

class bitmap::opaque {
public:
  int width, height;
  int channel;
  uptr<char[]> buffer;
};

int bitmap::width() const noexcept {
  assert(opaque_ != nullptr);

  return opaque_->width;
}

int bitmap::height() const noexcept {
  assert(opaque_ != nullptr);

  return opaque_->height;
}

const char *bitmap::buffer() const noexcept {
  assert(opaque_ != nullptr);

  return opaque_->buffer.get();
}

bool bitmap::load_from_file(std::string_view file) noexcept {
  assert(opaque_ != nullptr);
  if (FILE *pfile = fopen(file.data(), "rb"); pfile) {
    unsigned char *pbuffer = stbi_load_from_file(pfile,
                                                 &opaque_->width,
                                                 &opaque_->height,
                                                 &opaque_->channel,
                                                 0);
    if (nullptr != pbuffer) {
      size_t size = opaque_->width * opaque_->height * opaque_->channel;
      opaque_->buffer = make_uptr<char[]>(size);
      std::memcpy(opaque_->buffer.get(), pbuffer, size);
      stbi_image_free(pbuffer);
      return true;
    } else {
      std::cout << stbi_failure_reason() << std::endl;
    }
  }

  return false;
}

bool bitmap::load(const char *buffer, size_t size) noexcept {
  assert(opaque_ != nullptr);

  unsigned char *pbuffer = stbi_load_from_memory(reinterpret_cast<const unsigned char *>(buffer),
                                                 size,
                                                 &opaque_->width,
                                                 &opaque_->height,
                                                 &opaque_->channel,
                                                 0);
  if (nullptr != pbuffer) {
    size_t size = opaque_->width * opaque_->height * opaque_->channel;
    opaque_->buffer = make_uptr<char[]>(size);
    std::memcpy(opaque_->buffer.get(), pbuffer, size);
    stbi_image_free(pbuffer);
    return true;
  } else {
    std::cout << stbi_failure_reason() << std::endl;
  }

  return false;
}

bitmap::bitmap() noexcept : opaque_{make_uptr<opaque>()} {}

bitmap::~bitmap() noexcept {}

} // namespace core

} // namespace esim
