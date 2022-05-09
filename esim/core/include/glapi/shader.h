#ifndef __ESIM_CORE_GLAPI_SHADER_H_
#define __ESIM_CORE_GLAPI_SHADER_H_

#include "core/utils.h"
#include "debug.h"
#include <fstream>
#include <glad/glad.h>
#include <string>
#include <string_view>
#include <unordered_set>

namespace esim {

namespace gl {

/**
 * @brief Encapsulated OpenGL shader object.
 * 
 */
class shader {
public:
  typedef std::unordered_set<rptr<shader>> dep_type;

  /**
   * @brief Compile the shader by input content.
   * 
   * @param content specifies the text content to compile.
   * @return true if compiled successfully, false otherwise.
   */
  bool compile(std::string_view content) noexcept;

  /**
   * @brief Compile the shader by input file.
   * 
   * @param file specifies the path to the file.
   * @return true if compiled successfully, false otherwise.
   */
  bool compile_from_file(std::string_view file) noexcept;

  /**
   * @brief Obtain the shader type of shader.
   * 
   * @return the shader type.
   */
  GLenum type() const noexcept;

  /**
   * @brief Obtain the id of shader object.
   * 
   * @return a id to the shader object. 
   */
  GLuint id() const noexcept;
  
  /**
   * @brief Includes the other shader object.
   * 
   * @param dependency specifies the target shader object.
   */
  void include(rptr<shader> dependency) noexcept;

  /**
   * @brief Obtain the dependencies of the shader object.
   * 
   * @return a set of shader ids.
   */
  std::unordered_set<GLuint> dependencies() const noexcept;

  /**
   * @brief Construct a new shader object.
   *
   * @param type specifies the shader type.
   * Must be one of GL_COMPUTE_SHADER, GL_VERTEX_SHADER,
   * GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER,
   * GL_GEOMETRY_SHADER, or GL_FRAGMENT_SHADER.
   */
  shader(GLenum type) noexcept;

  /**
   * @brief Destroy the shader object.
   * 
   */
  ~shader() noexcept;

private:
  static std::string read(std::string_view file) noexcept;

private:
  const GLenum shader_type_;
  GLuint       id_;
  dep_type     dependencies_;
};

} // namespace gl

} // namespace esim

#include "shader.inl"

#endif