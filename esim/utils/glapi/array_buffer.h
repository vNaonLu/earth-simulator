#ifndef __ESIM_UTILS_GLAPI_ARRAY_BUFFER_H_
#define __ESIM_UTILS_GLAPI_ARRAY_BUFFER_H_

#include "glad/glad.h"
#include <cassert>
#include <vector>

namespace esim {

namespace gl {

/**
 * @brief OpenGL Vertex Array Object
 * 
 * @note Memorizes the vertex parameter bound
 */
class vertex_array {
public:
  /**
   * @brief Generate a vertex array object from OpenGL
   * 
   * @param count specifies the count of VAO. Default by 1.
   */
  inline void generate(size_t count = 1) noexcept {
    assert(count > 0);
    if (ids_.size() > 0) {
      glDeleteVertexArrays(ids_.size(), ids_.data());
    }
    ids_.resize(count);
    glGenVertexArrays(count, ids_.data());
  }

  /**
   * @brief Bind the vertex array object
   * 
   * @param count specifies the target index of VAOs. Default by 0.
   */
  inline void bind(size_t count = 0) const noexcept {
    assert(count < ids_.size());
    glBindVertexArray(ids_[count]);
  }
  
  /**
   * @brief Unbind the vertex array object
   * 
   */
  inline void unbind() const noexcept {
    glBindVertexArray(0);
  }

  /**
   * @brief Construct a new vertex array object
   * 
   */
  vertex_array() noexcept {}

  /**
   * @brief Destroy the vertex array object
   * 
   */
  ~vertex_array() noexcept {
    if (ids_.size() > 0) {
      glDeleteVertexArrays(ids_.size(), ids_.data());
    }
  }

private:
  std::vector<GLuint> ids_;
};

} // namespace gl

} // namespace esim

#endif