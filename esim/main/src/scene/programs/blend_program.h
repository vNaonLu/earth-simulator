#ifndef __ESIM_MAIN_SOURCE_SCENE_PROGRAMS_BLEND_PROGRAM_H_
#define __ESIM_MAIN_SOURCE_SCENE_PROGRAMS_BLEND_PROGRAM_H_

#include "core/utils.h"
#include "glapi/program.h"
#include <glm/vec2.hpp>

namespace esim {

namespace details {

struct blend_pos {
  glm::vec2 pos;
};

} // namespace details

namespace program {

class blend_program final : public gl::program {
public:
  typedef details::blend_pos vertex_type;

  static rptr<blend_program> get() noexcept;

  template <typename type>
  void update_resolution_uniform(type &&val) noexcept;

  template <typename type>
  void update_ndc_sun_uniform(type &&val) noexcept;

  void update_exposure_uniform(float val) noexcept;

  void update_gamma_uniform(float val) noexcept;

  void update_enable_scattering_uniform(int val) noexcept;

  void enable_position_pointer() const noexcept;

  blend_program() noexcept;

  ~blend_program() noexcept;

private:
  gl::shader vshader_, fshader_;
  GLint      location_exposure_, location_gamma_, location_ndc_sun_,
             location_resolution_, location_enable_scattering_;
  GLint      location_pos_;
};

inline rptr<blend_program> blend_program::get() noexcept {
  static uptr<blend_program> single;
  if (nullptr == single) {
    single = make_uptr<blend_program>();
  }

  return single.get();
}

template <typename type>
inline void blend_program::update_ndc_sun_uniform(type &&val) noexcept {
  glUniform2fv(location_ndc_sun_, 1, glm::value_ptr(std::forward<type>(val)));
}

template <typename type>
inline void blend_program::update_resolution_uniform(type &&val) noexcept {
  glUniform2fv(location_resolution_, 1, glm::value_ptr(std::forward<type>(val)));
}

inline void blend_program::update_exposure_uniform(float val) noexcept {
  glUniform1f(location_exposure_, val);
}

inline void blend_program::update_gamma_uniform(float val) noexcept {
  glUniform1f(location_gamma_, val);
}

inline void blend_program::update_enable_scattering_uniform(int val) noexcept {
  glUniform1i(location_enable_scattering_, val);
}

inline void blend_program::enable_position_pointer() const noexcept {
  glEnableVertexAttribArray(location_pos_);
  glVertexAttribPointer(location_pos_, 2, GL_FLOAT, GL_FALSE,
                        sizeof(vertex_type), (void *)0);
}

inline blend_program::blend_program() noexcept
    : vshader_{GL_VERTEX_SHADER}, fshader_{GL_FRAGMENT_SHADER} {
  vshader_.compile_from_file("assets/glsl/blend.vert");
  fshader_.compile_from_file("assets/glsl/blend.frag");
  assert(link_shaders(vshader_, fshader_));

  location_exposure_ = uniform_location("u_Exposure");
  location_gamma_ = uniform_location("u_Gamma");
  location_ndc_sun_ = uniform_location("u_LightNDCSunPos");
  location_resolution_ = uniform_location("u_Resolution");
  location_enable_scattering_ = uniform_location("u_EnableScattering");
  
  location_pos_ = attribute_location("a_Pos");
}

inline blend_program::~blend_program() noexcept {}

} // namespace program

} // namespace esim

#endif