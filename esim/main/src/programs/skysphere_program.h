#ifndef __ESIM_MAIN_SOURCE_SCENE_PROGRAMS_SKYSPHERE_PROGRAM_H_
#define __ESIM_MAIN_SOURCE_SCENE_PROGRAMS_SKYSPHERE_PROGRAM_H_

#include "common_program.h"

namespace esim {

namespace details {

struct skysphere_vertex {
  glm::vec3 pos;
  glm::vec2 texcoord;
};

} // namespace details

namespace program {

class skysphere_program final : public gl::program {
public:
  typedef details::skysphere_vertex vertex_type;

  static rptr<skysphere_program> get() noexcept;
  
  template <typename type>
  void update_model_uniform(type &&val) const noexcept;
  
  template <typename type>
  void update_view_uniform(type &&val) const noexcept;
  
  template <typename type>
  void update_project_uniform(type &&val) const noexcept;

  void enable_position_pointer() const noexcept;

  void enable_texcoord_pointer() const noexcept;

  skysphere_program() noexcept;

  ~skysphere_program() noexcept;

private:
  gl::shader vshader_, fshader_;
  GLint      location_model_mat_, location_view_mat_, location_proj_mat_;
  GLint      location_pos_, location_texcoord_;
};

inline rptr<skysphere_program> skysphere_program::get() noexcept {
  static uptr<skysphere_program> single;
  if (nullptr == single) {
    single = make_uptr<skysphere_program>();
  }

  return single.get();
}

template <typename type>
inline void skysphere_program::update_model_uniform(type &&val) const noexcept {
  assert(-1 != location_model_mat_);
  glUniformMatrix4fv(location_model_mat_, 1, GL_FALSE,
                     glm::value_ptr(std::forward<type>(val)));
}

template <typename type>
inline void skysphere_program::update_view_uniform(type &&val) const noexcept {
  assert(-1 != location_view_mat_);
  glUniformMatrix4fv(location_view_mat_, 1, GL_FALSE,
                     glm::value_ptr(std::forward<type>(val)));
}

template <typename type>
inline void skysphere_program::update_project_uniform(type &&val) const noexcept {
  assert(-1 != location_proj_mat_);
  glUniformMatrix4fv(location_proj_mat_, 1, GL_FALSE,
                     glm::value_ptr(std::forward<type>(val)));
}

inline void skysphere_program::enable_position_pointer() const noexcept {
  glEnableVertexAttribArray(location_pos_);
  glVertexAttribPointer(location_pos_, 3, GL_FLOAT, GL_FALSE,
                        sizeof(vertex_type), (void *)0);
}

inline void skysphere_program::enable_texcoord_pointer() const noexcept {
  glEnableVertexAttribArray(location_texcoord_);
  glVertexAttribPointer(location_texcoord_, 2, GL_FLOAT, GL_FALSE,
                        sizeof(vertex_type),
                        (void *)(sizeof(vertex_type::pos)));
}

inline skysphere_program::skysphere_program() noexcept
    : vshader_{GL_VERTEX_SHADER}, fshader_{GL_FRAGMENT_SHADER} {
  vshader_.compile_from_file("assets/glsl/outer_space.vert");
  fshader_.compile_from_file("assets/glsl/outer_space.frag");
  assert(link_shaders(vshader_, fshader_));

  location_model_mat_ = uniform_location("u_Modl");
  location_view_mat_ = uniform_location("u_View");
  location_proj_mat_ = uniform_location("u_Proj");

  location_pos_ = attribute_location("a_Pos");
  location_texcoord_ = attribute_location("a_TexCoord");
}

inline skysphere_program::~skysphere_program() noexcept {}

} // namespace program

} // namespace esim

#endif