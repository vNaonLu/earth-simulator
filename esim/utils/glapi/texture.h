#ifndef __ESIM_UTILS_GLAPI_TEXTURE_H_
#define __ESIM_UTILS_GLAPI_TEXTURE_H_

#include "glad/glad.h"
#include "stb/stb_image.h"
#include <cassert>
#include <string_view>

namespace esim {

namespace gl {

class texture {
public:
  inline void generate() noexcept {
    glGenTextures(1, &id_);
  }

  inline void bind_from_file(std::string_view file) noexcept {
    assert(!file.empty());
    assert(id_ != 0);

    int width, height, nr_ch;
    unsigned char *data = stbi_load(file.data(), &width, &height, &nr_ch, 0);

    assert(nullptr != data);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
  }

  inline void bind() const noexcept {
    glBindTexture(GL_TEXTURE_2D, id_);
  }

  texture() noexcept : id_{0} {}

  ~texture() noexcept {
    if (0 != id_) {
      glDeleteTextures(1, &id_);
    }
  }

private:
  GLuint id_;
};

} // namespace gl

} // namespace esim

#endif