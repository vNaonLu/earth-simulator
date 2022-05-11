#ifndef __ESIM_MAIN_SOURCE_SCENE_PROGRAMS_COMMON_PROGRAM_H_
#define __ESIM_MAIN_SOURCE_SCENE_PROGRAMS_COMMON_PROGRAM_H_

#include "common_shaders.h"
#include "core/transform.h"
#include "details/information.h"
#include "glapi/program.h"
#include "glapi/shader.h"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace esim {

namespace program {

class common_program : public gl::program {
public:
  using program::use;

  template <typename shader_type, typename... shaders_type>
  bool link_shader_and_common_shaders(shader_type &&sdr, shaders_type&&... sdrs) noexcept;

  void update_common_uniform(const scene::frame_info &info) const noexcept;
  
  template <typename type>
  void update_model_uniform(type &&val) const noexcept;

  void set_kr(float value) noexcept;

  void set_km(float value) noexcept;
  
  void set_esun(float value) noexcept;

  common_program() noexcept;

  virtual ~common_program() = default;

protected:
  using program::attribute_location;
  using program::uniform_location;

private:
  GLint      location_camera_, location_sun_dir_,
             location_invwave_, location_oradius_, location_iradius_,
             location_krsun_, location_kmsun_, location_kr4pi_, location_km4pi_,
             location_scale_, location_scale_depth_, location_g_,
             location_scale_over_scale_depth_,
             location_model_mat_, location_view_mat_, location_proj_mat_;

  float      value_kr_, value_km_, value_esun_;
};

template <typename shader_type, typename... shaders_type>
inline bool common_program::link_shader_and_common_shaders(shader_type &&sdr,
                                                           shaders_type &&...sdrs) noexcept {
  auto vshader = common_shader<GL_VERTEX_SHADER>::get();
  auto fshader = common_shader<GL_FRAGMENT_SHADER>::get();

  if (link_shaders(*vshader, *fshader,
                   std::forward<shader_type>(sdr),
                   std::forward<shaders_type>(sdrs)...)) {
    
    location_camera_ = uniform_location("u_CameraPos");
    location_sun_dir_ = uniform_location("u_LightDir");
    location_invwave_ = uniform_location("u_InvWavelength");
    location_oradius_ = uniform_location("u_OuterRadius");
    location_iradius_ = uniform_location("u_InnerRadius");
    location_krsun_ = uniform_location("u_KrESun");
    location_kmsun_ = uniform_location("u_KmESun");
    location_kr4pi_ = uniform_location("u_Kr4PI");
    location_km4pi_ = uniform_location("u_Km4PI");
    location_scale_ = uniform_location("u_Scale");
    location_scale_depth_ = uniform_location("u_ScaleDepth");
    location_scale_over_scale_depth_ = uniform_location("u_ScaleOverScaleDepth");
    location_g_ = uniform_location("u_g");
    location_model_mat_ = uniform_location("u_Modl");
    location_view_mat_ = uniform_location("u_View");
    location_proj_mat_ = uniform_location("u_Proj");

    return true;
  }

  return false;
}

inline void 
common_program::update_common_uniform(const scene::frame_info &info) const noexcept {
  using namespace glm;
  constexpr static float o_radius = astron::earth_major<float>() * 1.025f;
  constexpr static float i_radius = astron::earth_major<float>();
  constexpr static float ray_scale_depth = 0.25f;
  constexpr static float mie_scale_depth = 0.25f;
  constexpr static vec3  wave_length {0.650f, 0.570f, 0.475f};
  static vec3 inv_quat_wlength = 1.0f / pow(wave_length, vec3{4.0f});

  auto &cmr = info.camera;
  auto &sun = info.sun;

  glUniform3fv(location_camera_,  1, value_ptr(cmr.pos<float>()));
  glUniform3fv(location_sun_dir_, 1, value_ptr(sun.direction<float>()));

  glUniform3fv(location_invwave_, 1, value_ptr(inv_quat_wlength));
  glUniform1f(location_oradius_, o_radius);
  glUniform1f(location_iradius_, i_radius);
  glUniform1f(location_krsun_, value_kr_ * value_esun_);
  glUniform1f(location_kmsun_, value_km_ * value_esun_);
  glUniform1f(location_kr4pi_, value_kr_ * pi<float>() * 4.0f);
  glUniform1f(location_km4pi_, value_km_ * pi<float>() * 4.0f);
  glUniform1f(location_scale_, 1.0f / (o_radius - i_radius));
  glUniform1f(location_scale_depth_, ray_scale_depth);
  glUniform1f(location_scale_over_scale_depth_, 1.0f / (o_radius - i_radius) * ray_scale_depth);
  glUniform1f(location_g_, 0.999f);

  glUniformMatrix4fv(location_view_mat_, 1, GL_FALSE, value_ptr(cmr.view<float>()));
  glUniformMatrix4fv(location_proj_mat_, 1, GL_FALSE, value_ptr(cmr.project<float>()));
}

template <typename type>
inline void common_program::update_model_uniform(type &&val) const noexcept {
  assert(-1 != location_model_mat_);
  glUniformMatrix4fv(location_model_mat_, 1, GL_FALSE,
                     glm::value_ptr(std::forward<type>(val)));
}

inline void common_program::set_kr(float value) noexcept {
  value_kr_ = value;
}

inline void common_program::set_km(float value) noexcept {
  value_km_ = value;
}

inline void common_program::set_esun(float value) noexcept {
  value_esun_ = value;
}

inline common_program::common_program() noexcept
    : value_kr_{0.0015f}, value_km_{0.0010f}, value_esun_{10.5f} {
}

} // namespace shader

} // namespace esim

#endif