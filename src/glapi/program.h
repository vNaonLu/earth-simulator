#ifndef __ESIM_GLAPI_PROGRAM_H_
#define __ESIM_GLAPI_PROGRAM_H_

#include "buffer_object.h"
#include "utils.h"
#include <cassert>
#include <functional>
#include <glad/glad.h>
#include <memory>
#include <string_view>
#include <vector>

namespace esim {

namespace gl {

class program {
public:
  /**
   * @brief Use the program
   * 
   */
  inline void use() const noexcept {
    assert(id_ != 0, "program has been not initialized");
    glUseProgram(id_);
  }

  /**
   * @brief Obtain the uniform location
   * 
   * @param ufm_name specifies the uniform name.
   * @return the uniform location.
   */
  inline GLint uniform(std::string_view ufm_name) const noexcept {
    assert(id_ != 0);

    return glGetUniformLocation(id_, ufm_name.data());
  }

  /**
   * @brief Obtain the attribute location
   * 
   * @param att_name specifies the attribute name.
   * @return the attribute location.
   */
  inline GLint attribute(std::string_view att_name) const noexcept {
    assert(id_ != 0, "program has been not initialized");

    return glGetAttribLocation(id_, att_name.data());
  }

  /**
   * @brief Link shader program
   * 
   * @param shader specifies the shader.
   * @param shaders specifies the shaders.
   * @return true if link successfully, false otherwise.
   */
  template <typename shader_type, typename... shaders_type>
  inline bool link_shaders(shader_type &&shader, shaders_type &&...shaders) noexcept {
    attach<shader_type, shaders_type...>(std::forward<shader_type>(shader),
                                         std::forward<shaders_type>(shaders)...);
    glLinkProgram(id_);
    GLint code;
    glGetProgramiv(id_, GL_LINK_STATUS, &code);
    if (code == GL_FALSE) {
      std::vector<char> error_msg;
      GLint maxLength = 0;
      glGetProgramiv(id_, GL_INFO_LOG_LENGTH, &maxLength);

      error_msg.resize(maxLength + 1, 0);
      glGetProgramInfoLog(id_, maxLength, &maxLength, error_msg.data());
      error_call(error_msg.data());

      glDeleteProgram(id_);
      id_ = 0;

      return false;
    }

    return true;
  }

  /**
   * @brief Construct a new program object
   * 
   * @param error_cb specifies the error callback.
   */
  program(std::function<void(const char *)> error_cb = nullptr) noexcept
      : id_{0}, error_callback_{error_cb} {
    id_ = glCreateProgram();
  }

  ~program() noexcept {
    if (id_ != 0) {
      glDeleteProgram(id_);
      id_ = 0;
    }
  }

private:
  inline void error_call(const char *msg) const {
    if (nullptr != error_callback_) {
      error_callback_(msg);
    }
  }

  template <typename shader1, typename shader2, typename... shaders>
  inline void attach(shader1 &&s1, shader2 &&s2, shaders&&... ss) noexcept {
    assert(id_ != 0, "program has been not initialized");
    assert(s1.id() != 0, "shader has not been inititaled");
    glAttachShader(id_, s1.id());
    attach<shader2, shaders...>(std::forward<shader2>(s2), std::forward<shaders>(ss)...);
  }
  
  template <typename shader1>
  inline void attach(shader1 &&s1) noexcept {
    assert(id_ != 0, "program has been not initialized");
    assert(s1.id() != 0, "shader has not been inititaled");
    glAttachShader(id_, s1.id());
  }

private:
  GLuint id_;
  std::function<void(const char *)> error_callback_;

};

} // namespace gl

} // namespace esim

#endif