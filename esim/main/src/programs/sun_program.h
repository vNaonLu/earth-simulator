#ifndef __ESIM_MAIN_SOURCE_SCENE_PROGRAMS_SUN_PROGRAM_H_
#define __ESIM_MAIN_SOURCE_SCENE_PROGRAMS_SUN_PROGRAM_H_

#include "common_program.h"

namespace esim {

namespace details {

struct sun_vertex {
  glm::vec2 pos;
};

} // namespace details

namespace program {

class sun_program final : public gl::program {
public:
  typedef details::sun_vertex vertex_type;

  static rptr<sun_program> get() noexcept;
  
  template <typename type>
  void update_model_uniform(type &&val) const noexcept;
  
  template <typename type>
  void update_view_uniform(type &&val) const noexcept;
  
  template <typename type>
  void update_project_uniform(type &&val) const noexcept;
  
  template <typename type>
  void update_viewport_uniform(type &&val) const noexcept;
  
  template <typename type>
  void update_solar_color_uniform(type &&val) const noexcept;
  
  template <typename type>
  void update_pixel_size_uniform(type &&val) const noexcept;
  
  template <typename type>
  void update_solar_location_uniform(type &&val) const noexcept;

  void enable_position_pointer() const noexcept;

  sun_program() noexcept;

  ~sun_program() noexcept;

private:
  gl::shader vshader_, fshader_;
  GLint      location_model_mat_, location_view_mat_, location_proj_mat_,
             location_solar_color_, location_pixel_radius_, location_viewport_,
             location_solar_location_;
  GLint      location_pos_;
};

inline rptr<sun_program> sun_program::get() noexcept {
  static uptr<sun_program> single;
  if (nullptr == single) {
    single = make_uptr<sun_program>();
  }

  return single.get();
}

template <typename type>
inline void sun_program::update_model_uniform(type &&val) const noexcept {
  assert(-1 != location_model_mat_);
  glUniformMatrix4fv(location_model_mat_, 1, GL_FALSE,
                     glm::value_ptr(std::forward<type>(val)));
}

template <typename type>
inline void sun_program::update_view_uniform(type &&val) const noexcept {
  assert(-1 != location_view_mat_);
  glUniformMatrix4fv(location_view_mat_, 1, GL_FALSE,
                     glm::value_ptr(std::forward<type>(val)));
}

template <typename type>
inline void sun_program::update_viewport_uniform(type &&val) const noexcept {
  assert(-1 != location_viewport_);
  glUniform2fv(location_viewport_, 1,
               glm::value_ptr(std::forward<type>(val)));
}

template <typename type>
inline void sun_program::update_project_uniform(type &&val) const noexcept {
  assert(-1 != location_proj_mat_);
  glUniformMatrix4fv(location_proj_mat_, 1, GL_FALSE,
                     glm::value_ptr(std::forward<type>(val)));
}

template <typename type>
inline void sun_program::update_solar_color_uniform(type &&val) const noexcept {
  assert(-1 != location_solar_color_);
  glUniform4fv(location_solar_color_, 1,
               glm::value_ptr(std::forward<type>(val)));
}

template <typename type>
inline void sun_program::update_pixel_size_uniform(type &&val) const noexcept {
  assert(-1 != location_pixel_radius_);
  glUniform1f(location_pixel_radius_, std::forward<type>(val));
}

template <typename type>
inline void sun_program::update_solar_location_uniform(type &&val) const noexcept {
  assert(-1 != location_solar_location_);
  glUniform4fv(location_solar_location_, 1, glm::value_ptr(std::forward<type>(val)));
}

inline void sun_program::enable_position_pointer() const noexcept {
  glEnableVertexAttribArray(location_pos_);
  glVertexAttribPointer(location_pos_, 2, GL_FLOAT, GL_FALSE,
                        sizeof(vertex_type), (void *)0);
}

inline sun_program::sun_program() noexcept
    : vshader_{GL_VERTEX_SHADER}, fshader_{GL_FRAGMENT_SHADER} {
  vshader_.compile_from_file("assets/glsl/sun.vert");
  fshader_.compile_from_file("assets/glsl/sun.frag");
  assert(link_shaders(vshader_, fshader_));

  location_model_mat_ = uniform_location("u_Modl");
  location_view_mat_ = uniform_location("u_View");
  location_proj_mat_ = uniform_location("u_Proj");
  location_solar_color_ = uniform_location("u_SolarColor");
  location_pixel_radius_ = uniform_location("u_PixelRadius");
  location_solar_location_ = uniform_location("u_SolorNDCPos");
  location_viewport_ = uniform_location("u_Resolution");

  location_pos_ = attribute_location("a_Pos");
}

inline sun_program::~sun_program() noexcept {}

} // namespace program

} // namespace esim

#endif