#ifndef __ESIM_MAIN_SOURCE_SCENE_PROGRAM_SURFACE_PROGRAM_H_
#define __ESIM_MAIN_SOURCE_SCENE_PROGRAM_SURFACE_PROGRAM_H_

#include "common_program.h"

namespace esim {

namespace details {

struct surface_vertex {
  glm::vec3 pos;
  glm::vec3 normal;
  glm::vec2 texcoord;
};

} // namespace details

namespace program {

class surface_program final : public common_program {
public:
  typedef details::surface_vertex vertex_type;

  static rptr<surface_program> get() noexcept;

  void enable_position_pointer() const noexcept;

  void enable_normal_pointer() const noexcept;
  
  void enable_texcoord_pointer() const noexcept;

  surface_program() noexcept;

  ~surface_program() noexcept;

private:
  gl::shader vshader_, fshader_;
  GLint location_pos_, location_normal_, location_texcoord_;
};

inline rptr<surface_program> surface_program::get() noexcept {
  static uptr<surface_program> single;
  if (nullptr == single) {
    single = make_uptr<surface_program>();
  }

  return single.get();
}

inline void surface_program::enable_position_pointer() const noexcept {
  glEnableVertexAttribArray(location_pos_);
  glVertexAttribPointer(location_pos_, 3, GL_FLOAT, GL_FALSE,
                        sizeof(vertex_type), (void *)0);
}

inline void surface_program::enable_normal_pointer() const noexcept {
  glEnableVertexAttribArray(location_normal_);
  glVertexAttribPointer(location_normal_, 3, GL_FLOAT, GL_FALSE,
                        sizeof(vertex_type),
                        (void *)(sizeof(vertex_type::pos)));
}

inline void surface_program::enable_texcoord_pointer() const noexcept {
  glEnableVertexAttribArray(location_texcoord_);
  glVertexAttribPointer(location_texcoord_, 2, GL_FLOAT, GL_FALSE,
                        sizeof(vertex_type),
                        (void *)(sizeof(vertex_type::pos) + sizeof(vertex_type::normal)));
}

inline surface_program::surface_program() noexcept
    : vshader_{GL_VERTEX_SHADER}, fshader_{GL_FRAGMENT_SHADER} {
  vshader_.compile_from_file("assets/glsl/surface.vert");
  fshader_.compile_from_file("assets/glsl/surface.frag");
  assert(link_shader_and_common_shaders(vshader_, fshader_));

  location_pos_      = attribute_location("a_Pos");
  location_normal_   = attribute_location("a_Normal");
  location_texcoord_ = attribute_location("a_TexCoord");
}

inline surface_program::~surface_program() noexcept {}

} // namespace program

} // namespace esim

#endif