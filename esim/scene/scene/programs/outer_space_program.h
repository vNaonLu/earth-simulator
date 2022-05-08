#ifndef __ESIM_SCENE_SCENE_PROGRAMS_OUTER_SPACE_PROGRAM_H_
#define __ESIM_SCENE_SCENE_PROGRAMS_OUTER_SPACE_PROGRAM_H_

#include "glapi/program.h"
#include "glapi/shader.h"
#include "glm/vec3.hpp"
#include "scene/render/rendering_infos.h"
#include "transform/wgs84.h"
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
struct outer_space_vertex {
  glm::vec3 pos;
  glm::vec2 texcoord;
};

class outer_space_program final : public gl::program {
public:
  inline void bind_common_uniform(const rendering_infos& info) const noexcept {
    using namespace glm;
    
    auto &cmr = info.camera;
    glUniformMatrix4fv(u_view_, 1, GL_FALSE, glm::value_ptr(cmr.view()));
    glUniformMatrix4fv(u_proj_, 1, GL_FALSE, glm::value_ptr(cmr.skysphere_projection()));
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
    assert(-1 != a_pos_);
    glEnableVertexAttribArray(a_pos_);
    glVertexAttribPointer(a_pos_, 3,
                          GL_FLOAT, GL_FALSE,
                          sizeof(outer_space_vertex), (void *)0);
  }
  
  /**
   * @brief Enable texCoord vertex array and bind pointer with
   * current bound VBO
   *
   */
  inline void enable_texture_coord_pointer() const noexcept {
    assert(-1 != a_tex_);
    glEnableVertexAttribArray(a_tex_);
    glVertexAttribPointer(a_tex_, 2,
                          GL_FLOAT, GL_FALSE,
                          sizeof(outer_space_vertex), (void *)(sizeof(float) * 3));
  }

  /**
   * @brief Construct a new surface program object
   * 
   * @param err_cb specifies the error callback.
   * @note Do not consturct shader program directly.
   */
  outer_space_program(gl_error_callback err_cb = nullptr) noexcept
      : gl::program{err_cb},
        vert_{GL_VERTEX_SHADER}, frag_{GL_FRAGMENT_SHADER},
        u_model_{-1}, u_view_{-1}, u_proj_{-1},
        a_pos_{-1}, a_tex_{-1},
        error_msg_{err_cb} {
    std::string vs_text, fs_text;
    assert(read_file("glsl/outer_space.vert", vs_text));
    assert(read_file("glsl/outer_space.frag", fs_text));
    assert(vert_.compile(vs_text));
    assert(frag_.compile(fs_text));
    assert(link_shaders(vert_, frag_));

    u_model_ = uniform("u_Modl"); u_view_ = uniform("u_View"); u_proj_ = uniform("u_Proj");

    a_pos_ = attribute("a_Pos"); a_tex_ = attribute("a_TexCoord");
  }

  /**
   * @brief Obtain the Surface Shader
   * 
   * @return a pointer to surface shader.
   */
  inline static r_ptr<outer_space_program> get() noexcept {
    static u_ptr<outer_space_program> shader__;

    if (nullptr != shader__) {

      return shader__.get();
    } else {
      shader__ = make_ptr_u<outer_space_program>();

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
  gl::shader vert_, frag_;
  GLint      u_model_, u_view_, u_proj_;
  GLint      a_pos_, a_tex_;
  gl_error_callback error_msg_;
};

} // namespace scene

} // namespace esim

#endif