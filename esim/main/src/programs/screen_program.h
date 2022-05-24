#ifndef __ESIM_MAIN_SOURCE_SCENE_PROGRAMS_SCREEN_PROGRAM_H_
#define __ESIM_MAIN_SOURCE_SCENE_PROGRAMS_SCREEN_PROGRAM_H_

#include "core/utils.h"
#include "glapi/program.h"
#include <glm/vec2.hpp>

namespace esim {

namespace details {

struct screen_vertex {
  glm::vec2 pos;
};

} // namespace details

namespace program {

class screen_program final : public gl::program {
public:
  typedef details::screen_vertex vertex_type;

  static rptr<screen_program> get() noexcept;

  void update_alpha_uniform(float val) noexcept;

  void enable_position_pointer() const noexcept;

  screen_program() noexcept;

  ~screen_program() noexcept;

private:
  gl::shader vshader_, fshader_;
  GLint      location_alpha_;
  GLint      location_pos_;
};

inline rptr<screen_program> screen_program::get() noexcept {
  static uptr<screen_program> single;
  if (nullptr == single) {
    single = make_uptr<screen_program>();
  }

  return single.get();
}

inline void screen_program::update_alpha_uniform(float val) noexcept {
  glUniform1f(location_alpha_, val);
}

inline void screen_program::enable_position_pointer() const noexcept {
  glEnableVertexAttribArray(location_pos_);
  glVertexAttribPointer(location_pos_, 2, GL_FLOAT, GL_FALSE,
                        sizeof(vertex_type), (void *)0);
}

inline screen_program::screen_program() noexcept
    : vshader_{GL_VERTEX_SHADER}, fshader_{GL_FRAGMENT_SHADER} {
  vshader_.compile_from_file("assets/glsl/screen.vert");
  fshader_.compile_from_file("assets/glsl/screen.frag");
  assert(link_shaders(vshader_, fshader_));

  location_alpha_ = uniform_location("u_Alpha");
  
  location_pos_ = attribute_location("a_Pos");
}

inline screen_program::~screen_program() noexcept {}

} // namespace program

} // namespace esim

#endif