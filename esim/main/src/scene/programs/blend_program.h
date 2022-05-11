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

  void update_exposure_uniform(float val) noexcept;

  void update_gamma_uniform(float val) noexcept;

  void enable_position_pointer() const noexcept;

  blend_program() noexcept;

  ~blend_program() noexcept;

private:
  gl::shader vshader_, fshader_;
  GLint      location_exposure_, location_gamma_;
  GLint      location_pos_;
};

inline rptr<blend_program> blend_program::get() noexcept {
  static uptr<blend_program> single;
  if (nullptr == single) {
    single = make_uptr<blend_program>();
  }

  return single.get();
}

inline void blend_program::update_exposure_uniform(float val) noexcept {
  glUniform1f(location_exposure_, val);
}

inline void blend_program::update_gamma_uniform(float val) noexcept {
  glUniform1f(location_gamma_, val);
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
  
  location_pos_ = attribute_location("a_Pos");
}

inline blend_program::~blend_program() noexcept {}

} // namespace program

} // namespace esim

#endif