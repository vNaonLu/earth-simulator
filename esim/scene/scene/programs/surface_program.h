#ifndef __ESIM_SCENE_SCENE_PROGRAMS_SURFACE_PROGRAM_H_
#define __ESIM_SCENE_SCENE_PROGRAMS_SURFACE_PROGRAM_H_

#include "glapi/program.h"
#include "glapi/shader.h"
#include "utils/camera.h"
#include "utils/utils.h"
#include <fstream>
#include <glad/glad.h>
#include <memory>

namespace esim {

namespace scene {

/**
 * @brief The vertex details of surface shader
 * 
 */
struct surface_node_vertex {
  float x, y, z;
  float tx, ty;
};

class surface_program final : public gl::program {
public:
  /**
   * @brief Bind view perspective uniform 
   * 
   * @param val specifies the vec3 value.
   */
  template <typename type>
  inline void bind_offset_uniform(type &&val) const noexcept {
    glUniform3fv(u_offset_, 1,
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
                          sizeof(surface_node_vertex), (void *)0);
  }
  
  /**
   * @brief Enable texCoord vertex array and bind pointer with
   * current bound VBO
   *
   */
  inline void enable_texture_coord_pointer() const noexcept {
    glEnableVertexAttribArray(a_tex_);
    glVertexAttribPointer(a_tex_, 2,
                          GL_FLOAT, GL_FALSE,
                          sizeof(surface_node_vertex), (void *)(sizeof(float) * 3));
  }

  /**
   * @brief Construct a new surface program object
   * 
   * @param err_cb specifies the error callback.
   * @note Do not consturct shader program directly.
   */
  surface_program(gl_error_callback err_cb = nullptr) noexcept
      : gl::program{err_cb},
        vert_{GL_VERTEX_SHADER},
        frag_{GL_FRAGMENT_SHADER},
        u_model_{-1}, u_offset_{-1}, a_pos_{-1},
        error_msg_{err_cb} {
    std::string vs_text, fs_text;
    assert(read_file("glsl/surface.vs", vs_text));
    assert(read_file("glsl/surface.fs", fs_text));
    assert(vert_.compile(vs_text));
    assert(frag_.compile(fs_text));
    assert(link_shaders(vert_, frag_));
    u_model_ = uniform("unfm_model");
    u_view_ = uniform("unfm_view");
    u_proj_ = uniform("unfm_proj");
    u_offset_ = uniform("unfm_offset");
    a_pos_ = attribute("attb_pos");
    a_tex_ = attribute("attb_text");
  }

  /**
   * @brief Obtain the Surface Shader
   * 
   * @return a pointer to surface shader.
   */
  inline static r_ptr<surface_program> get() noexcept {
    static u_ptr<surface_program> shader__;

    if (nullptr != shader__) {

      return shader__.get();
    } else {
      shader__ = make_ptr_u<surface_program>();

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
    }
  }

private:
  gl::shader vert_,
             frag_;
  GLint      u_model_, u_view_, u_proj_, u_offset_;
  GLint      a_pos_, a_tex_;
  gl_error_callback error_msg_;
};

} // namespace scene

} // namespace esim

#endif