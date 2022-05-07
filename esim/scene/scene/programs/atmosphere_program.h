#ifndef __ESIM_SCENE_SCENE_PROGRAMS_ATMOSPHERE_PROGRAM_H_
#define __ESIM_SCENE_SCENE_PROGRAMS_ATMOSPHERE_PROGRAM_H_

#include "glapi/program.h"
#include "glapi/shader.h"
#include "glm/vec3.hpp"
#include "utils/camera.h"
#include "utils/utils.h"
#include <fstream>
#include <glad/glad.h>
#include <memory>

namespace esim {

namespace scene {

/**
 * @brief The vertex details of atmosphere shader
 * 
 */
struct atmosphere_vertex {
  glm::vec3 pos;
};

class atmosphere_program final : public gl::program {
public:
  /**
   * @brief Bind view depth uniform 
   * 
   * @param val specifies the float value.
   */
  inline void bind_view_depth_uniform(GLfloat val) const noexcept {
    glUniform1f(u_view_depth_, val);
  }
  
  /**
   * @brief Bind scatter color coefficients uniform 
   * 
   * @param val specifies the vec4 value.
   * @note r, g, b, ambient
   */
  template <typename type>
  inline void bind_scatter_coefficient_uniform(type &&val) const noexcept {
    glUniform4fv(u_scatter_, 1,
                 glm::value_ptr(std::forward<type>(val)));
  }

  /**
   * 
   * @brief Bind solar direction uniform 
   * 
   * @param val specifies the vec3 value.
   */
  template <typename type>
  inline void bind_solor_dir_uniform(type &&val) const noexcept {
    glUniform3fv(u_solor_dir_, 1,
                 glm::value_ptr(std::forward<type>(val)));
  }

  /**
   * @brief Bind perspective matrix uniform 
   * 
   * @param val specifies the mat4x4 value.
   */
  template <typename type>
  inline void bind_proj_uniform(type &&val) const noexcept {
    glUniformMatrix4fv(u_proj_, 1, GL_FALSE,
                       glm::value_ptr(std::forward<type>(val)));
  }
  
  /**
   * @brief Bind view matrix uniform 
   * 
   * @param val specifies the mat4x4 value.
   */
  template <typename type>
  inline void bind_view_uniform(type &&val) const noexcept {
    glUniformMatrix4fv(u_view_, 1, GL_FALSE,
                       glm::value_ptr(std::forward<type>(val)));
  }


  /**
   * @brief Bind model uniform 
   * 
   * @param val specifies the mat4x4 value.
   */
  template <typename type>
  inline void bind_model_uniform(type &&val) const noexcept {
    glUniformMatrix4fv(u_model_, 1, GL_FALSE,
                       glm::value_ptr(std::forward<type>(val)));
  }

  /**
   * @brief Enable position vertex array and bind pointer with
   * current bound VBO
   *
   */
  inline void enable_position_pointer() const noexcept {
    glEnableVertexAttribArray(a_pos_);
    glVertexAttribPointer(a_pos_, 3,
                          GL_FLOAT, GL_FALSE,
                          sizeof(atmosphere_vertex), (void *)0);
  }

  /**
   * @brief Construct a new surface program object
   * 
   * @param err_cb specifies the error callback.
   * @note Do not consturct shader program directly.
   */
  atmosphere_program(gl_error_callback err_cb = nullptr) noexcept
      : gl::program{err_cb},
        vert_{GL_VERTEX_SHADER},
        frag_{GL_FRAGMENT_SHADER},
        u_model_{-1}, u_solor_dir_{-1}, u_scatter_{-1},
        u_view_depth_{-1}, a_pos_{-1},
        error_msg_{err_cb} {
    std::string vs_text, fs_text;
    assert(read_file("glsl/atmosphere.vert", vs_text));
    assert(read_file("glsl/atmosphere.frag", fs_text));
    assert(vert_.compile(vs_text));
    assert(frag_.compile(fs_text));
    assert(link_shaders(vert_, frag_));
    u_model_ = uniform("unfm_model");
    u_view_ = uniform("unfm_view");
    u_proj_ = uniform("unfm_proj");
    u_solor_dir_ = uniform("unfm_solar_dir");
    u_scatter_ = uniform("unfm_scatter_coefficients");
    u_view_depth_ = uniform("unfm_view_depth");
    a_pos_ = attribute("attb_pos");
  }

  /**
   * @brief Obtain the Atmosphere Shader
   * 
   * @return a pointer to Atmosphere shader.
   */
  inline static r_ptr<atmosphere_program> get() noexcept {
    static u_ptr<atmosphere_program> shader__;

    if (nullptr != shader__) {

      return shader__.get();
    } else {
      shader__ = make_ptr_u<atmosphere_program>();

      return shader__.get();
    }
  }

private:
  static bool read_file(std::string_view path, std::string &buf) noexcept {
    std::ifstream file(path.data());
    if (file.is_open()) {
      buf = std::string(std::istreambuf_iterator<char>(file),
                        std::istreambuf_iterator<char>());

      return true;
    }
    return false;
  }

  void error(const char *msg) const {
    if (nullptr != error_msg_) {
      error_msg_(msg);
    } else {
      std::cout << msg << std::endl;
    }
  }

private:
  gl::shader        vert_,
                    frag_;
  GLint             u_model_, u_view_, u_proj_, u_solor_dir_,
                    u_scatter_, u_view_depth_;
  GLint             a_pos_;
  gl_error_callback error_msg_;
};

} // namespace scene

} // namespace esim

#endif