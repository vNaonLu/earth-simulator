#ifndef __ESIM_UTILS_GLAPI_SHADER_H_
#define __ESIM_UTILS_GLAPI_SHADER_H_

#include "utils/utils.h"
#include <cassert>
#include <functional>
#include <glad/glad.h>
#include <string>
#include <string_view>

namespace esim {

namespace gl {

class program;

class shader {
public:
  /**
   * @brief Compile the shader with input text
   * 
   * @param text specifies the shader text.
   * @return true if shader successfully compiled, false otherwise.
   */
  inline bool compile(std::string_view text) noexcept {
    shader_content_ = text;
    const GLint text_size = (GLint)text.size();
    r_ptr<const GLchar> data = shader_content_.data();
    glShaderSource(id_, 1, &data, &text_size);
    glCompileShader(id_);

    GLint code;
    glGetShaderiv(id_, GL_COMPILE_STATUS, &code);
    if (code == GL_FALSE) {
      std::vector<char> error_msg;
      GLint max_length = 0;
      glGetShaderiv(id_, GL_INFO_LOG_LENGTH, &max_length);

      error_msg.resize(max_length + 1, 0);
      glGetShaderInfoLog(id_, max_length, &max_length, error_msg.data());
      error_call(error_msg.data());
      glDeleteShader(id_);
      id_ = 0;

      return false;
    }

    return true;
  }

  /**
   * @brief Construct a new shader object
   *
   * @param shader_type specifies the shader type.
   * Must be one of GL_COMPUTE_SHADER, GL_VERTEX_SHADER,
   * GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER,
   * GL_GEOMETRY_SHADER, or GL_FRAGMENT_SHADER.\
   * @param error_cb specifies the error callback.
   */
  shader(GLenum shader_type,
         std::function<void(const char *)> error_cb = nullptr) noexcept
      : id_{0}, error_callback_{error_cb} {
    id_ = glCreateShader(shader_type);
  }

  ~shader() noexcept {
    if (id_ != 0) {
      glDeleteShader(id_);
      id_ = 0;
    }
  }

private:
  inline void error_call(const char *msg) const {
    std::cout << msg << std::endl;
    if (nullptr != error_callback_) {
      error_callback_(msg);
    }
  }

  inline GLuint id() const noexcept {

    return id_;
  }

private:
  GLuint id_;
  std::string shader_content_;
  std::function<void(const char *)> error_callback_;

  friend class program;
};

} // namespace gl

} // namespace esim

#endif