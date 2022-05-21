#ifndef __ESIM_ESIM_SOURCE_DETAILS_SURFACE_VERTEX_ENGINE_H_
#define __ESIM_ESIM_SOURCE_DETAILS_SURFACE_VERTEX_ENGINE_H_

#include "core/bounding_box.h"
#include "core/transform.h"
#include "programs/bounding_box_program.h"
#include "programs/surface_program.h"

namespace esim {

class surface_vertices {
public:
  typedef details::surface_vertex      vbo_buffer_type;
  typedef details::bounding_box_vertex obb_buffer_type;
  typedef struct {
    glm::dvec3 pos, normal;
    glm::dvec2 texcoord;
  } vertex_type;

  const glm::dvec3 &offset() const noexcept;

  const core::bounding_box &obb() const noexcept;

  double tile_radius() const noexcept;

  void calculate() noexcept;

  std::vector<vbo_buffer_type> export_buffer() const noexcept;

  std::vector<obb_buffer_type> export_obb_buffer() const noexcept;

  surface_vertices(const geo::maptile &tile, uint32_t details) noexcept;

  ~surface_vertices() = default;

private:
  void calculate_center() noexcept;

  void calculate_skirt() noexcept;

  void calculate_normal() noexcept;

private:
  const geo::maptile      &tile_;
  const uint32_t           vertex_details_;
  glm::dvec3               offset_;
  double                   tile_radius_;
  std::vector<vertex_type> buffer_;
  uptr<core::bounding_box> obb_;
};

class surface_vertex_engine final {
public:
  uptr<surface_vertices> gen_surface_vertices(const geo::maptile &tile) const noexcept;

  std::vector<uint16_t> export_center_element_buffer() const noexcept;

  std::vector<uint16_t> export_skirt_element_buffer() const noexcept;

  std::vector<uint16_t> export_obb_element_buffer() const noexcept;

  surface_vertex_engine(uint32_t details) noexcept;

  ~surface_vertex_engine() = default;

private:
  uint32_t vertex_details_;
};

} // namespace esim

#endif