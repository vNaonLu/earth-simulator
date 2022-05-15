#ifndef __ESIM_MAIN_SOURCE_SCENE_PROGRAMS_ATMOSPHERE_PROGRAM_H_
#define __ESIM_MAIN_SOURCE_SCENE_PROGRAMS_ATMOSPHERE_PROGRAM_H_

#include "common_program.h"

namespace esim {

namespace details {

struct atmosphere_vertex {
  glm::vec3 pos;
};

} // namespace details

namespace program {

class atmosphere_program final : public common_program {
public:
  typedef details::atmosphere_vertex vertex_type;

  static rptr<atmosphere_program> get() noexcept;

  void enable_position_pointer() const noexcept;

  atmosphere_program() noexcept;

  ~atmosphere_program() noexcept;

private:
  gl::shader vshader_, fshader_;
  GLint location_pos_;
};

inline rptr<atmosphere_program> atmosphere_program::get() noexcept {
  static uptr<atmosphere_program> single;
  if (nullptr == single) {
    single = make_uptr<atmosphere_program>();
  }

  return single.get();
}

inline void atmosphere_program::enable_position_pointer() const noexcept {
  glEnableVertexAttribArray(location_pos_);
  glVertexAttribPointer(location_pos_, 3, GL_FLOAT, GL_FALSE,
                        sizeof(vertex_type), (void *)0);
}

inline atmosphere_program::atmosphere_program() noexcept
    : vshader_{GL_VERTEX_SHADER}, fshader_{GL_FRAGMENT_SHADER} {
  vshader_.compile_from_file("assets/glsl/atmosphere.vert");
  fshader_.compile_from_file("assets/glsl/atmosphere.frag");
  assert(link_shader_and_common_shaders(vshader_, fshader_));

  location_pos_ = attribute_location("a_Pos");
  
  set_kr(0.0025f);
  set_km(0.0015f);
  set_esun(1.0f);
}

inline atmosphere_program::~atmosphere_program() noexcept {}

} // namespace program

} // namespace esim

#endif