#ifndef __ESIM_GLAPI_BUFFER_OBJECT_H_
#define __ESIM_GLAPI_BUFFER_OBJECT_H_

#include "gl_helper.h"
#include <cstdint>
#include <vector>

namespace esim {

namespace gl {

namespace details {

class buffer_base {
public:
  /**
   * @brief Generate a GL Buffer Object
   * 
   * @param count specifies the buffer count. Default by 1.
   */
  inline void generate_buffer(size_t count = 1) noexcept {
    assert(count > 0);
    if (ids_.size() > 0) {
      glDeleteBuffers(ids_.size(), ids_.data());
    }
    ids_.resize(count);
    glGenBuffers(count, ids_.data());
  }

protected:
  inline void bind_buffer(GLenum type = GL_ARRAY_BUFFER, size_t idx = 0) const noexcept {
    assert(idx < ids_.size());
    glBindBuffer(type, ids_[idx]);
  }

  buffer_base() noexcept : ids_{0} {
  }

  ~buffer_base() noexcept {
    if (!ids_.empty()) {
      glDeleteBuffers(ids_.size(), ids_.data());
      ids_.clear();
    }
  }

private:
  std::vector<GLuint> ids_;

};

} // namespace details

/**
 * @brief Index Buffer Object
 * 
 * @tparam base_type specifies the data type.
 */
template <typename base_type = uint32_t>
class index_buffer : public details::buffer_base {
public:
  typedef base_type type;
  inline static constexpr size_t type_size = sizeof(type);

  static_assert(std::is_integral_v<type>, "type must be integral type.");

  /**
   * @brief Bind the Buffer Data and withing move data side effect
   * 
   * @param data specifies the target data to bind. 
   * @param usage specifies the usage of buffer.
   * @param idx specifies the index of buffer.
   */
  inline void bind_buffer_data(std::vector<type> &data,
                               GLenum usage = GL_STATIC_DRAW,
                               size_t idx = 0) noexcept {
    assert(!data.empty());
    GLint orig_bound_id;
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &orig_bound_id);
    bind(idx);
    buffer_ = std::move(data);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 type_size * buffer_.size(),
                 buffer_.data(), usage);
    if (0 != orig_bound_id) {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_BINDING, orig_bound_id);
    }
  }

  /**
   * @brief Bind Index Buffer
   * 
   * @param idx specifies the target index to bind. Default by 0.
   */
  inline void bind(size_t idx = 0) const noexcept {
    bind_buffer(GL_ELEMENT_ARRAY_BUFFER, idx);
  }

  /**
   * @brief Obtain buffer size count
   * 
   * @return size_t the count of buffer.
   */
  inline size_t size() const noexcept {

    return buffer_.size();
  }
  
  /**
   * @brief Get buffer element
   * 
   * @param idx specifies the target index.
   * @return the reference to buffer element.
   */
  inline const base_type &at(size_t idx) const noexcept {

    return buffer_.at(idx);
  }

private:
  std::vector<base_type> buffer_;
};

/**
 * @brief Vertex Buffer Object
 * 
 * @tparam base_type specifies the data type.
 */
template <typename base_type = float>
class vertex_buffer : public details::buffer_base {
public:
  typedef base_type type;
  inline static constexpr size_t type_size = sizeof(type);

  /**
   * @brief Bind the Buffer Data and withing move data side effect
   * 
   * @param data specifies the target data to bind. 
   * @param usage specifies the usage of buffer.
   * @param idx specifies the index of buffer.
   */
  inline void bind_buffer_data(std::vector<type> &data,
                               GLenum usage = GL_STATIC_DRAW,
                               size_t idx = 0) noexcept {
    assert(!data.empty());
    GLint orig_bound_id;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &orig_bound_id);
    bind(idx);
    buffer_ = std::move(data);
    glBufferData(GL_ARRAY_BUFFER,
                 type_size * buffer_.size(),
                 buffer_.data(), usage);
    if (0 != orig_bound_id) {
      glBindBuffer(GL_ARRAY_BUFFER, orig_bound_id);
    }
  }

  /**
   * @brief Bind Index Buffer
   * 
   * @param idx specifies the target index to bind. Default by 0.
   */
  inline void bind(size_t idx = 0) const noexcept {
    bind_buffer(GL_ARRAY_BUFFER, idx);
  }

  /**
   * @brief Obtain buffer size count
   * 
   * @return size_t the count of buffer.
   */
  inline size_t size() const noexcept {

    return buffer_.size();
  }

  /**
   * @brief Get buffer element
   * 
   * @param idx specifies the target index.
   * @return the reference to buffer element.
   */
  inline const base_type &at(size_t idx) const noexcept {

    return buffer_.at(idx);
  }

private:
  std::vector<base_type> buffer_;
};

} // namespace gl

} // namespace esim

#endif