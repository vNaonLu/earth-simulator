#ifndef __ESIM_MAIN_SOURCE_SCENE_PROGRAM_BOUNDING_BOX_PROGRAM_H_
#define __ESIM_MAIN_SOURCE_SCENE_PROGRAM_BOUNDING_BOX_PROGRAM_H_

#include "common_program.h"

namespace esim {

namespace details {

struct bounding_box_vertex {
  glm::vec3 pos;
};

} // namespace details

namespace program {

class bounding_box_program final : public common_program {
public:
  typedef details::bounding_box_vertex vertex_type;

  static rptr<bounding_box_program> get() noexcept;

  template <typename type>
  void update_line_color_uniform(type && val) const noexcept;

  void enable_position_pointer() const noexcept;

  bounding_box_program() noexcept;

  ~bounding_box_program() noexcept;

private:
  gl::shader vshader_, fshader_;
  GLint  location_line_color_;
  GLint location_pos_;
};

inline rptr<bounding_box_program> bounding_box_program::get() noexcept {
  static uptr<bounding_box_program> single;
  if (nullptr == single) {
    single = make_uptr<bounding_box_program>();
  }

  return single.get();
}

template <typename type>
inline void bounding_box_program::update_line_color_uniform(type &&val) const noexcept {
  glUniform4fv(location_line_color_, 1, glm::value_ptr(std::forward<type>(val)));
}

inline void bounding_box_program::enable_position_pointer() const noexcept {
  glEnableVertexAttribArray(location_pos_);
  glVertexAttribPointer(location_pos_, 3, GL_FLOAT, GL_FALSE,
                        sizeof(vertex_type), (void *)0);
}

inline bounding_box_program::bounding_box_program() noexcept
    : vshader_{GL_VERTEX_SHADER}, fshader_{GL_FRAGMENT_SHADER} {
  vshader_.compile_from_file("assets/glsl/bounding_box.vert");
  fshader_.compile_from_file("assets/glsl/bounding_box.frag");
  assert(link_shader_and_common_shaders(vshader_, fshader_));

  location_line_color_ = uniform_location("u_LineColor");
  location_pos_ = attribute_location("a_Pos");
}

inline bounding_box_program::~bounding_box_program() noexcept {}

} // namespace program

} // namespace esim

#endif