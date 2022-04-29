#ifndef __ESIM_GLAPI_BUFFER_OBJECT_H_
#define __ESIM_GLAPI_BUFFER_OBJECT_H_

#include "gl_helper.h"
#include <cstdint>
#include <vector>

namespace esim {

namespace details {

class gl_buffer_base {
public:
  /**
   * @brief Generate a GL Buffer Object
   * 
   * @param count specifies the buffer count. Default by 1.
   */
  inline void generate_buffer(size_t count = 1) noexcept {
    assert(count > 0);
    ids_.resize(count);
    glGenBuffers(count, ids_.data());
  }

protected:
  inline void bind_buffer(GLenum type = GL_ARRAY_BUFFER, size_t idx = 0) const noexcept {
    assert(idx < ids_.size());
    glBindBuffer(type, ids_[idx]);
  }

  gl_buffer_base() noexcept : ids_{0} {
  }

  ~gl_buffer_base() noexcept {
    if (!ids_.empty()) {
      glDeleteBuffers(ids_.size(), ids_.data());
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
class gl_index_buffer : public details::gl_buffer_base {
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
    std::cout << "size: " << buffer_.size() << std::endl;
    std::cout << "bytes: " << (type_size * buffer_.size()) << std::endl;
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

private:
  std::vector<base_type> buffer_;
};

/**
 * @brief Vertex Buffer Object
 * 
 * @tparam base_type specifies the data type.
 */
template <typename base_type = float>
class gl_vertex_buffer : public details::gl_buffer_base {
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
    std::cout << "size: " << buffer_.size() << std::endl;
    std::cout << "bytes: " << (type_size * buffer_.size()) << std::endl;
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

private:
  std::vector<base_type> buffer_;
};

} // namespace esim

#endif