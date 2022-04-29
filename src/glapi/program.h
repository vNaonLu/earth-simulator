#ifndef __ESIM_GLAPI_PROGRAM_H_
#define __ESIM_GLAPI_PROGRAM_H_

#include "../utils.h"
#include <cassert>
#include <functional>
#include <glad/glad.h>
#include <memory>
#include <string_view>
#include <vector>

namespace esim {

class gl_program {
public:
  inline bool is_ready() const noexcept {

    return ready_;
  }

  inline void use_program() const noexcept {
    assert(ready_);
    glUseProgram(h_program_);
  }

  inline GLint obtain_uniform(std::string_view name) const noexcept {
    assert(ready_);
    GLint res = glGetUniformLocation(h_program_, name.data());

    return res;
  }
  
  inline GLint obtain_attribute(std::string_view name) const noexcept {
    assert(ready_);
    GLint res = glGetAttribLocation(h_program_, name.data());
    glEnableVertexAttribArray(res);

    return res;
  }

  gl_program(std::string_view vert_text,
             std::string_view frag_text,
             std::function<void(const char *)> error_msg = nullptr) noexcept
      : ready_{false},
        h_program_{0}, h_vert_{0}, h_frag_{0},
        error_callback_{error_msg} {
    if (auto res = compile_shader<GL_VERTEX_SHADER>(h_vert_, vert_text); !res.empty()) {
      error_call(res.data());

      return;
    }
    if (auto res = compile_shader<GL_FRAGMENT_SHADER>(h_frag_, frag_text); !res.empty()) {
      error_call(res.data());

      return;
    }
    if (auto res = link_program(h_program_, h_vert_, h_frag_); !res.empty()) {
      error_call(res.data());

      return;
    }

    ready_ = true;
  }

  ~gl_program() noexcept {
    if (h_program_ != 0) {
      glDeleteProgram(h_program_);
      h_program_ = 0;
    }

    if (h_vert_ != 0) {
      glDeleteShader(h_vert_);
      h_vert_ = 0;
    }

    if (h_frag_ != 0) {
      glDeleteShader(h_frag_);
      h_frag_ = 0;
    }
  }

private:
  inline void error_call(const char *msg) const {
    if (nullptr != error_callback_) {
      error_callback_(msg);
    }
  }

  template <typename handle_type>
  inline static void attach_shaders(GLuint h_prog, handle_type h_shader) {
    glAttachShader(h_prog, h_shader);
  }

  template <typename handle_type, typename next_handle_type, typename... handles_type>
  inline static void attach_shaders(GLuint h_prog,
                                    handle_type h_shader,
                                    next_handle_type h_next,
                                    handles_type &&... hs_) {
    glAttachShader(h_prog, h_shader);
    attach_shaders<next_handle_type, handles_type...>(
        h_prog, h_next, std::forward<handles_type>(hs_)...);
  }

  template<typename... handles_type>
  inline static std::vector<char> link_program(GLuint &h_prog, handles_type&&... h_shaders) noexcept {
    std::vector<char> error_msg;
    h_prog = glCreateProgram();
    attach_shaders(h_prog, std::forward<handles_type>(h_shaders)...);
    glLinkProgram(h_prog);

    GLint code;
    glGetProgramiv(h_prog, GL_LINK_STATUS, &code);
    if (code == GL_FALSE) {
      GLint maxLength = 0;
      glGetProgramiv(h_prog, GL_INFO_LOG_LENGTH, &maxLength);

      error_msg.resize(maxLength + 1, 0);
      glGetProgramInfoLog(h_prog, maxLength, &maxLength, error_msg.data());

      glDeleteProgram(h_prog);
      h_prog = 0;
    }

    return error_msg;
  }

  template <GLenum type>
  inline static std::vector<char> compile_shader(GLuint &handle, std::string_view text) noexcept {
    std::vector<char> error_msg;
    GLint text_size = static_cast<GLint>(text.size());
    handle = glCreateShader(type);
    r_ptr<const GLchar> data = text.data();
    glShaderSource(handle, 1, &data, &text_size);
    glCompileShader(handle);

    GLint code;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &code);
    if (code == GL_FALSE) {
      GLint max_length = 0;
      glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &max_length);

      error_msg.resize(max_length + 1, 0);
      glGetShaderInfoLog(handle, max_length, &max_length, error_msg.data());
      glDeleteShader(handle);
      handle = 0;
    }

    return error_msg;
  }

private:
  bool ready_;
  GLuint h_program_;
  GLuint h_vert_, h_frag_;
  std::function<void(const char *)> error_callback_;
};

} // namespace esim

#endif