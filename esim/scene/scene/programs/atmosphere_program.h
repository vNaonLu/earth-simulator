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
  inline void bind_common_uniform(const rendering_infos& info) const noexcept {
    using namespace glm;
    constexpr static float kr = 0.0025f;
    constexpr static float km = 0.0010f;
    constexpr static float esun = 1.0f;

    constexpr static float outer_radius = static_cast<float>(trans::WGS84_A * 1.025);
    constexpr static float inner_radius = static_cast<float>(trans::WGS84_A); 
    constexpr static float ray_scale_depth = 0.25f;
    constexpr static float mie_scale_depth = 0.1f;
    constexpr static vec3  wave_length = {0.650f, 0.570f, 0.475f};
    vec3  inv_quat_wavelength = 1.0f / glm::pow(wave_length, vec3(4.0f));

    auto &cmr = info.camera;
    auto &sun = info.sun;

    glUniform3fv(u_camera_pos_, 1, glm::value_ptr(static_cast<vec3>(cmr.ecef())));
    glUniform3fv(u_light_dir_, 1, glm::value_ptr(static_cast<vec3>(sun.direction())));

    glUniform3fv(u_inv_wave_length_, 1, glm::value_ptr(inv_quat_wavelength));
    glUniform1f(u_outer_radius_, outer_radius);
    glUniform1f(u_inner_radius_, inner_radius);
    glUniform1f(u_kr_esun_, kr * esun);
    glUniform1f(u_km_esun_, km * esun);
    glUniform1f(u_kr4pi_, km * pi<float>() * 4.0f);
    glUniform1f(u_km4pi_, km * pi<float>() * 4.0f);
    glUniform1f(u_scale_, 1.0f / (outer_radius - inner_radius));
    glUniform1f(u_scale_depth_, ray_scale_depth);
    glUniform1f(u_scale_over_scale_depth_, 1.0f / (outer_radius - inner_radius) * ray_scale_depth);
    glUniform1f(u_g_, 0.999f);

    glUniformMatrix4fv(u_view_, 1, GL_FALSE, glm::value_ptr(cmr.view()));
    glUniformMatrix4fv(u_proj_, 1, GL_FALSE, glm::value_ptr(cmr.projection()));
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
        common_vert_{GL_VERTEX_SHADER}, common_frag_{GL_FRAGMENT_SHADER},
        vert_{GL_VERTEX_SHADER}, frag_{GL_FRAGMENT_SHADER},
        u_camera_pos_{-1}, u_light_dir_{-1}, u_inv_wave_length_{-1}, u_outer_radius_{-1},
        u_inner_radius_{-1}, u_kr_esun_{-1}, u_km_esun_{-1}, u_kr4pi_{-1}, u_km4pi_{-1}, u_scale_{-1},
        u_scale_depth_{-1}, u_scale_over_scale_depth_{-1}, u_g_{-1},
        a_pos_{-1}, error_msg_{err_cb} {
    std::string common_text, vs_text, fs_text;
    assert(read_file("glsl/common.glsl", common_text));
    assert(read_file("glsl/atmosphere.vert", vs_text));
    assert(read_file("glsl/atmosphere.frag", fs_text));
    assert(common_vert_.compile(common_text));
    assert(common_frag_.compile(common_text));
    assert(vert_.compile(vs_text));
    assert(frag_.compile(fs_text));
    assert(link_shaders(vert_, frag_, common_vert_, common_frag_));

    u_model_ = uniform("u_Modl"); u_view_ = uniform("u_View"); u_proj_ = uniform("u_Proj");
    
    u_camera_pos_ = uniform("u_CameraPos"); u_light_dir_ = uniform("u_LightDir"); u_inv_wave_length_ = uniform("u_InvWavelength");
    u_outer_radius_ = uniform("u_OuterRadius"); u_inner_radius_ = uniform("u_InnerRadius");
    u_kr_esun_ = uniform("u_KrESun"); u_km_esun_ = uniform("u_KmESun"); u_kr4pi_ = uniform("u_Kr4PI"); u_km4pi_ = uniform("u_Km4PI");
    u_scale_ = uniform("u_Scale"); u_scale_depth_ = uniform("u_ScaleDepth");
    u_scale_over_scale_depth_ = uniform("u_ScaleOverScaleDepth"); u_g_ = uniform("u_g");

    a_pos_ = attribute("a_Pos");
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
    std::cout << msg << std::endl;
    if (nullptr != error_msg_) {
      error_msg_(msg);
    }
  }

private:
  gl::shader common_vert_, common_frag_, vert_, frag_;
  GLint      u_camera_pos_, u_light_dir_, u_inv_wave_length_, u_outer_radius_,
             u_inner_radius_, u_kr_esun_, u_km_esun_, u_kr4pi_, u_km4pi_, u_scale_,
             u_scale_depth_, u_scale_over_scale_depth_, u_g_;
  GLint      u_model_, u_view_, u_proj_;
  GLint      a_pos_;
  gl_error_callback error_msg_;
};

} // namespace scene

} // namespace esim

#endif