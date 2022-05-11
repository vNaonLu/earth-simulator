#ifndef __ESIM_CORE_GLAPI_TEXTURE_H_
#define __ESIM_CORE_GLAPI_TEXTURE_H_

#include <cassert>
#include <glad/glad.h>
#include <stb/stb_image.h>
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
   */
  bool load(std::string_view file, size_t idx = 0) const noexcept;

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

private:
  std::vector<GLuint> ids_;
};

} // namespace gl

} // namespace esim

#include "texture.inl"

#endif