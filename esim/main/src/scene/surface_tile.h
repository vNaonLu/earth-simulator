#ifndef __ESIM_MAIN_SOURCE_SCENE_SURFACE_TILE_H_
#define __ESIM_MAIN_SOURCE_SCENE_SURFACE_TILE_H_

#include "core/bounding_box.h"
#include "core/transform.h"
#include "details/information.h"
#include "details/surface_vertex_engine.h"
#include "glapi/buffer.h"
#include "glapi/texture.h"
#include "programs/bounding_box_program.h"
#include "programs/surface_program.h"
#include <array>

namespace esim {

namespace scene {

class surface_tile final {
public:
  const geo::maptile &details() const noexcept;

  void gen_vertex_buffer(uptr<surface_vertices> generator) noexcept;

  bool is_ready_to_render() const noexcept;

  void render(const scene::frame_info &info, size_t indices_count) noexcept;

  void render_bounding_box(const scene::frame_info &info, size_t indices_count) noexcept;

  surface_tile(geo::maptile tile) noexcept;

  ~surface_tile() = default;

  std::pair<bool, bool> is_enough_resolution(const scene::frame_info &info) const noexcept;

  bool is_visible(const scene::frame_info &info) const noexcept;

  std::array<rptr<surface_tile>, 4> expand() noexcept;

  rptr<surface_tile> collapse() noexcept;

private:
  void before_render() noexcept;

private:
  const geo::maptile                        info_;
  bool                                      ready_to_render_, buffer_generated_;
  glm::dvec3                                offset_;
  gl::texture                               basemap_;
  uptr<surface_vertices>                    vertices_generator_;
  uptr<gl::buffer<details::surface_vertex>>      vbo_;
  uptr<gl::buffer<details::bounding_box_vertex>> obb_vbo_;

  rptr<surface_tile>                parent_;
  std::array<uptr<surface_tile>, 4> children_;
};

} // namespace scene

} // namespace esim

#endif