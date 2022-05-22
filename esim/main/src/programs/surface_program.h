#ifndef __ESIM_MAIN_SOURCE_SCENE_PROGRAM_SURFACE_PROGRAM_H_
#define __ESIM_MAIN_SOURCE_SCENE_PROGRAM_SURFACE_PROGRAM_H_

#include "common_program.h"
#include "details/basemap_storage.h"

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

  void update_basemap_uniform(rptr<basemap> bm, basemap_texinfo info) const noexcept;

  void enable_position_pointer() const noexcept;

  void enable_normal_pointer() const noexcept;
  
  void enable_texcoord_pointer() const noexcept;

  surface_program() noexcept;

  ~surface_program() noexcept;

private:
  gl::shader vshader_, fshader_;
  GLint location_use_basemap_, location_tex_offset_, location_tex_scale_;
  GLint location_pos_, location_normal_, location_texcoord_;
};

inline rptr<surface_program> surface_program::get() noexcept {
  static uptr<surface_program> single;
  if (nullptr == single) {
    single = make_uptr<surface_program>();
  }

  return single.get();
}

inline void surface_program::update_basemap_uniform(rptr<basemap> bm, basemap_texinfo info) const noexcept {
  if (nullptr == bm) {
    glUniform1i(location_use_basemap_, 0);
  } else {
    auto texture = bm->texture();
    if (nullptr != texture) {
      texture->bind();
      glUniform1i(location_use_basemap_, 1);
      glUniform1f(location_tex_scale_, info.scale);
      glUniform2fv(location_tex_offset_, 1, glm::value_ptr(info.offset));
    } else {
      glUniform1i(location_use_basemap_, 0);
    }
  }
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

  location_use_basemap_ = uniform_location("u_UseBaseMap");
  location_tex_offset_ = uniform_location("u_TexOffset");
  location_tex_scale_ = uniform_location("u_TexScale");

  location_pos_      = attribute_location("a_Pos");
  location_normal_   = attribute_location("a_Normal");
  location_texcoord_ = attribute_location("a_TexCoord");

  set_kr(0.0025f);
  set_km(0.0015f);
  set_esun(20.0f);
}

inline surface_program::~surface_program() noexcept {}

} // namespace program

} // namespace esim

#endif