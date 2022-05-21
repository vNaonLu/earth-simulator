#include "scene/surface_tile.h"
#include "core/bounding_box.h"
#include <cstdint>
#include <glad/glad.h>

namespace esim {

namespace scene {

const geo::maptile &surface_tile::details() const noexcept {

  return info_;
}

void surface_tile::gen_vertex_buffer(uptr<surface_vertices> generator) noexcept {
  assert(nullptr != generator);
  vertices_generator_ = std::move(generator);
  vertices_generator_->calculate();
  offset_ = vertices_generator_->offset();
  ready_to_render_ = true;
}

bool surface_tile::is_ready_to_render() const noexcept {

  return ready_to_render_;
}

void surface_tile::before_render() noexcept {
  if (!buffer_generated_) {
    vbo_ = make_uptr<gl::buffer<details::surface_vertex>>(GL_ARRAY_BUFFER, 2);
    vbo_->bind_buffer(vertices_generator_->export_buffer(), GL_STATIC_DRAW, 0);

    obb_vbo_ = make_uptr<gl::buffer<details::bounding_box_vertex>>(GL_ARRAY_BUFFER);
    obb_vbo_->bind_buffer(vertices_generator_->export_obb_buffer());

    buffer_generated_ = true;
  }
}

void surface_tile::render(const scene::frame_info &info,
                          size_t indices_count) noexcept {
  using namespace glm;
  before_render();
  auto &sun = info.sun;
  auto &cmr = info.camera;
  auto ERA = rotate(dmat4x4{1.0f}, astron::era<double>(sun.julian_date()), dvec3{0.0f, 0.0f, 1.0f});
  dvec3 offset_era = ERA * dvec4{offset_, 1.0};
  auto model = cmr.translate(dmat4x4{1.0f}, offset_era) * ERA;

  auto program = program::surface_program::get();
  vbo_->bind();
  program->enable_position_pointer();
  program->enable_normal_pointer();
  program->enable_texcoord_pointer();
  program->update_model_uniform(static_cast<mat4x4>(model));
  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices_count), GL_UNSIGNED_SHORT, nullptr);
}

void surface_tile::render_bounding_box(const scene::frame_info &info,
                                       size_t indices_count) noexcept {
  using namespace glm;
  before_render();
  auto &sun = info.sun;
  auto &cmr = info.camera;
  auto ERA = rotate(dmat4x4{1.0f}, astron::era<double>(sun.julian_date()), dvec3{0.0f, 0.0f, 1.0f});
  dvec3 offset_era = ERA * dvec4{offset_, 1.0};
  auto model = cmr.translate(dmat4x4{1.0f}, offset_era) * ERA;
  
  auto program = program::bounding_box_program::get();
  obb_vbo_->bind();
  program->enable_position_pointer();
  program->update_model_uniform(static_cast<mat4x4>(model));
  glDrawElements(GL_LINES, static_cast<GLsizei>(indices_count), GL_UNSIGNED_SHORT, nullptr);
}

surface_tile::surface_tile(geo::maptile tile) noexcept
    : info_{tile}, ready_to_render_{false}, buffer_generated_{false},
      offset_{0.0f}, parent_{nullptr} {
}

std::pair<bool, bool>
surface_tile::is_enough_resolution(const scene::frame_info &info) const noexcept {
  using namespace glm;
  std::pair<bool, bool> resolution_check{false, false};

  if (nullptr != vertices_generator_) {
    auto &cmr = info.camera;
    auto &sun = info.sun;
    auto ERA = rotate(dmat4x4{1.0f}, astron::era<double>(sun.julian_date()), dvec3{0.0f, 0.0f, 1.0f});
    dvec3 offset_era = ERA * dvec4{offset_, 1.0};

    double terrain_radius = vertices_generator_->tile_radius();
    auto P_matrix = cmr.project<float>();
    auto NDC = P_matrix * vec4{static_cast<float>(2.0 * terrain_radius),
                               static_cast<float>(2.0 * terrain_radius),
                               static_cast<float>(length(cmr.pos() - offset_era)), 1.0f};
    NDC /= NDC.w;
    NDC = abs(NDC);

    auto &[too_far, too_near] = resolution_check;
    too_near = info_.lod < 16 && ((NDC.x > 1.0f) || (NDC.y > 1.0f));
    too_far = info_.lod > 0 && ((NDC.x < 0.5f) || (NDC.y < 0.5f));
  }

  return resolution_check;
}

bool surface_tile::is_visible(const scene::frame_info &info) const noexcept {
  /// reference: https://cesium.com/blog/2013/04/25/horizon-culling/
  using namespace glm;
  constexpr static dvec3 base = {geo::wgs84::A, geo::wgs84::A, geo::wgs84::B};
  auto &cmr = info.camera;
  auto &sun = info.sun;
  auto ERA = rotate(dmat4x4{1.0f}, astron::era<double>(sun.julian_date()), dvec3{0.0f, 0.0f, 1.0f});
  dvec3 offset_era = ERA * dvec4{offset_, 1.0};
  dvec3 cv = cmr.pos() / base;
  dmat4x4 MVP = cmr.project() * cmr.view() * cmr.translate(dmat4x4{1.0f}, offset_era);

  double vh_magnitude_sq = dot(cv, cv) - 1.0f;
  auto &bounding_box = vertices_generator_->obb();

  for (auto &v : bounding_box.data()) {
    dvec3 pt = (v + offset_era) / base,
          vt = pt - cv;
    double vt_magnitude_sq = dot(vt, vt);
    double vt_dot_vc = -dot(cv, vt);

    if (!((vt_dot_vc > vh_magnitude_sq) &&
          (vt_dot_vc * vt_dot_vc / vt_magnitude_sq > vh_magnitude_sq))) {
      /// is not occluded

      return true;
    }
  }

  return false;
}

std::array<rptr<surface_tile>, 4> surface_tile::expand() noexcept {
  std::array<rptr<surface_tile>, 4> res;
  if (nullptr == children_.front()) {
    /// children generated at same time
    /// hence do check only once.
    geo::maptile child_info = info_;
    ++child_info.lod;
    child_info.x <<= 1;
    child_info.y <<= 1;
    std::array<geo::maptile, 4> children_info = {geo::maptile{child_info.lod, child_info.x, child_info.y},
                                                 geo::maptile{child_info.lod, child_info.x, child_info.y + 1},
                                                 geo::maptile{child_info.lod, child_info.x + 1, child_info.y},
                                                 geo::maptile{child_info.lod, child_info.x + 1, child_info.y + 1}};
    for (size_t i = 0; i < 4; ++i) {
      children_[i] = make_uptr<surface_tile>(children_info[i]);
      children_[i]->parent_ = this;
    }
  }

  for (size_t i = 0; i < 4; ++i) {
    res[i] = children_[i].get();
  }

  return res;
}

rptr<surface_tile> surface_tile::collapse() noexcept {

  return parent_;
}

} // namespace scene

} // namespace esim
