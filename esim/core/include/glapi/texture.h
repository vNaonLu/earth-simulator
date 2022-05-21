#ifndef __ESIM_CORE_GLAPI_TEXTURE_H_
#define __ESIM_CORE_GLAPI_TEXTURE_H_

#include "core/bitmap.h"
#include <cassert>
#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <iostream>
#include <string_view>
#include <vector>

namespace esim {

namespace gl {

/**
 * @brief Encapsulated OpenGL texture object.
 * 
 */
class texture {
public:
  /**
   * @brief The Optionals for texture
   * 
   */
  struct options {
    GLint wrap_s = GL_REPEAT;
    GLint wrap_t = GL_REPEAT;
    GLint min_filter = GL_LINEAR;
    GLint mag_filter = GL_LINEAR;
  };

  /**
   * @brief Bind the texture with specified index.
   * 
   * @param idx specifies the index of texture.
   * @param location specifies the location to bind.
   */
  void bind(size_t idx = 0, GLuint location = 0) const noexcept;

  /**
   * @brief Load a texture from a local image.
   * 
   * @param file specifies the target file location.
   * @param idx specifies the index of texture.
   * @param opt specifies the texture details.
   */
  bool load(std::string_view file, size_t idx = 0, options opt = options{}) noexcept;

  /**
   * @brief Load a texture from a bitmap object.
   * 
   * @param bm specifies the target bitmap.
   * @param idx specifies the index of texture.
   * @param opt specifies the texture details.
   */
  bool load(const core::bitmap &bm, size_t idx = 0, options opt = options{}) noexcept;

  /**
   * @brief Obtain the resolution of texture.
   * 
   * @param idx specifies the index of texture.
   * @return the 2-integer vector.
   */
  glm::ivec2 resolution(size_t idx = 0) const noexcept;

  /**
   * @brief Construct a new texture object.
   * 
   * @param count specifies the count of textures, default by 1.
   */
  texture(size_t count = 1) noexcept;

  /**
   * @brief Destroy the texture object.
   * 
   */
  ~texture() noexcept;

protected:
  std::vector<GLuint>     ids_;
  std::vector<glm::ivec2> resolution_;
};

} // namespace gl

} // namespace esim

#include "texture.inl"

#endif