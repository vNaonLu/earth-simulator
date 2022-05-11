#ifndef __ESIM_CORE_GLAPI_PROGRAM_H_
#define __ESIM_CORE_GLAPI_PROGRAM_H_

#include "core/utils.h"
#include "shader.h"
#include <cassert>
#include <glad/glad.h>
#include <iostream>
#include <unordered_set>

namespace esim {

namespace gl {

/**
 * @brief Encapsulated OpenGL program object.
 * 
 */
class program {
public:
  /**
   * @brief Use the program.
   * 
   */
  void use() const noexcept;

  /**
   * @brief Obtain the location of uniform by name.
   * 
   * @param variable specifies the uniform name.
   * @return the location to uniform. return -1 if optimized.
   */
  GLint uniform_location(std::string_view variable) const noexcept;

  /**
   * @brief Obtain the location of attribute by name.
   * 
   * @param variable specifies the uniform name.
   * @return the location to uniform. return -1 if optimized.
   */
  GLint attribute_location(std::string_view variable) const noexcept;

  /**
   * @brief Construct a new program object.
   * 
   */
  program() noexcept;

  /**
   * @brief Destroy the program object.
   * 
   */
  ~program() noexcept;

protected:
  template <typename shader_type, typename... shaders_type>
  bool link_shaders(shader_type &&sdr, shaders_type&&... sdrs) noexcept;

private:
  template <typename curr_sd, typename next_sd, typename... remains>
  void attach_shader(curr_sd &&sd1, next_sd &&sd2, remains&&... sds) noexcept;

  template <typename shader_type>
  void attach_shader(shader_type &&sdr) noexcept;

  template <typename shader_type>
  void attach_shader_recursive(shader_type &&sdr) noexcept;

private:
  GLuint id_;
  std::unordered_set<GLuint> attached_shaders_;
};

} // namespace gl

} // namespace esim

#include "program.inl"

#endif