#ifndef __ESIM_MAIN_SOURCE_SCENE_PROGRAMS_BLUR_PROGRAM_H_
#define __ESIM_MAIN_SOURCE_SCENE_PROGRAMS_BLUR_PROGRAM_H_

#include "core/utils.h"
#include "glapi/program.h"
#include <glm/vec2.hpp>

namespace esim {

namespace details {

struct blur_pos {
  glm::vec2 pos;
};

} // namespace details

namespace program {

class blur_program final : public gl::program {
public:
  typedef details::blur_pos vertex_type;

  static rptr<blur_program> get() noexcept;

  void update_horizontal_uniform(uint32_t val) noexcept;

  void enable_position_pointer() const noexcept;

  blur_program() noexcept;

  ~blur_program() noexcept;

private:
  gl::shader vshader_, fshader_;
  GLint      location_horizontal_;
  GLint      location_pos_;
};

inline rptr<blur_program> blur_program::get() noexcept {
  static uptr<blur_program> single;
  if (nullptr == single) {
    single = make_uptr<blur_program>();
  }

  return single.get();
}

inline void blur_program::update_horizontal_uniform(uint32_t val) noexcept {
  glUniform1i(location_horizontal_, static_cast<GLint>(val));
}

inline void blur_program::enable_position_pointer() const noexcept {
  glEnableVertexAttribArray(location_pos_);
  glVertexAttribPointer(location_pos_, 2, GL_FLOAT, GL_FALSE,
                        sizeof(vertex_type), (void *)0);
}

inline blur_program::blur_program() noexcept
    : vshader_{GL_VERTEX_SHADER}, fshader_{GL_FRAGMENT_SHADER} {
  vshader_.compile_from_file("assets/glsl/blur.vert");
  fshader_.compile_from_file("assets/glsl/blur.frag");
  assert(link_shaders(vshader_, fshader_));

  location_horizontal_ = uniform_location("u_Horizontal");
  
  location_pos_ = attribute_location("a_Pos");
}

inline blur_program::~blur_program() noexcept {}

} // namespace program

} // namespace esim

#endif