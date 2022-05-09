#ifndef __ESIM_CORE_GLAPI_BUFFER_H_
#define __ESIM_CORE_GLAPI_BUFFER_H_

#include <cassert>
#include <glad/glad.h>
#include <vector>

namespace esim {

namespace gl {

/**
 * @brief Encapsulated OpenGL buffer object.
 * 
 * @tparam type specifies the single vertex structure.
 */
template <typename type>
class buffer {
public:
  typedef type underlying_type;

  /**
   * @brief Obtain the buffer data bound.
   * 
   * @param idx specufies the index of the data.
   * @return const reference to bound buffer data. 
   */
  const std::vector<underlying_type> &data(size_t idx = 0) const noexcept;

  /**
   * @brief Obtain the size of buffer data.
   * 
   * @param idx specifies the index of buffer data.
   * @return the length of specified buffer data.
   */
  size_t size(size_t idx = 0) const noexcept;

  /**
   * @brief bind the buffer data.
   * 
   * @param idx specifies the index of buffer object.
   */
  void bind(size_t idx = 0) noexcept;

  /**
   * @brief Unbind the buffer data.
   * 
   */
  void unbind() const noexcept;

  /**
   * @brief bind the buffer data to buffer object.
   * 
   * @param data specifies the rvalue buffer data.
   * @param idx specifies the bound location.
   */
  void bind_buffer(std::vector<underlying_type> &&data,
                   GLenum usage = GL_STATIC_DRAW,
                   size_t idx = 0) noexcept;

  /**
   * @brief Obtain the type of buffer object.
   * 
   * @return specifies the buffer type.
   */
  GLenum type() const noexcept;

  /**
   * @brief Construct a new buffer object.
   * 
   * @param type specifies the buffer type.
   * @param count specifies the buffer size, default by 1.
   */
  buffer(GLenum type, size_t count = 1) noexcept;

  /**
   * @brief Destroy the buffer object.
   * 
   */
  ~buffer() noexcept;

private:
  const size_t                              buffer_size_;
  const GLenum                              buffer_type_;
  std::vector<GLuint>                       ids_;
  std::vector<std::vector<underlying_type>> data_;
};

} // namespace gl

} // namespace esim

#include "buffer.inl"

#endif