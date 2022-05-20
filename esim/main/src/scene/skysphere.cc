#include "scene/skysphere.h"
#include "core/transform.h"

namespace esim {

namespace scene {

void skysphere::render(const frame_info &info) noexcept {
  auto &cmr = info.camera;
  auto program = program::skysphere_program::get();
  program->use();
  skymap_.bind();
  ebo_.bind();
  vbo_.bind();
  program->enable_position_pointer();
  program->enable_texcoord_pointer();
  program->update_model_uniform(GC_rotation_);
  program->update_view_uniform(cmr.view<float>());
  program->update_project_uniform(cmr.skysphere_project<float>());
  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(ebo_.size()), GL_UNSIGNED_INT, nullptr);
}

skysphere::skysphere() noexcept
    : GC_rotation_{astron::mat_equator_to_galactic<float>()},
      offset_{0.0f},
      ebo_{GL_ELEMENT_ARRAY_BUFFER, 1},
      vbo_{GL_ARRAY_BUFFER, 1} {

  vbo_.bind_buffer(gen_vertex_buffer());
  ebo_.bind_buffer(gen_element_buffer());
  assert(skymap_.load("assets/img/starmap_g4k.jpg"));
}

skysphere::~skysphere() noexcept {}

namespace details {
constexpr static uint32_t latitude_details = 22;
constexpr static uint32_t longitude_details = 45;
constexpr static double   max_latitude = +90.0;
constexpr static double   min_latitude = -90.0;
constexpr static double   stride_latitude = (max_latitude - min_latitude) / latitude_details;
constexpr static double   max_longitude = +180.0;
constexpr static double   min_longitude = -180.0;
constexpr static double   stride_longitude = (max_longitude - min_longitude) / longitude_details;
} // namespace details

static uint32_t to_vertex_index(uint32_t row, uint32_t col) noexcept {

  return static_cast<uint32_t>(row * (details::longitude_details) + col);
}

std::vector<esim::details::skysphere_vertex> skysphere::gen_vertex_buffer() noexcept {
  using namespace glm;
  std::vector<esim::details::skysphere_vertex> buffer((details::latitude_details + 1) *
                                                      (details::longitude_details + 1));
  auto it = buffer.begin();
  float radius = sqrt(2.0f);
  for (size_t i = 0; i <= details::latitude_details; ++i) {
    for (size_t j = 0; j <= details::longitude_details; ++j) {
      auto &v = (*it).pos;
      auto &tx = (*it++).texcoord;
      dvec3 pos{radians(details::min_latitude + details::stride_latitude * i),
                radians(details::min_longitude + details::stride_longitude * j),
                0.0};
      double coslat = cos(pos.x), sinlat = sin(pos.x),
             coslon = cos(pos.y), sinlon = sin(pos.y);
      v.x = static_cast<float>(radius * coslat * coslon);
      v.y = static_cast<float>(radius * coslat * sinlon);
      v.z = static_cast<float>(radius * sinlat);
      tx.s = static_cast<float>((pos.y - radians(details::min_longitude)) / 2.0f / pi<double>());
      tx.t = static_cast<float>((pos.x - radians(details::min_latitude)) / pi<double>());
      offset_ += v;
    }
  }
  offset_ /= ((details::latitude_details + 1) * (details::longitude_details + 1));
  for (auto &v : buffer) {
    v.pos -= offset_;
  }

  return buffer;
}

std::vector<uint32_t> skysphere::gen_element_buffer() noexcept {
  using namespace glm;
  std::vector<uint32_t> buffer((details::latitude_details + 1) *
                               details::longitude_details * 6);
  auto it = buffer.begin();
  for (uint32_t i = 0; i <= details::latitude_details; ++i) {
    for (uint32_t j = 0; j < details::longitude_details; ++j) {
      *it++ = to_vertex_index(i, j);
      *it++ = to_vertex_index(i + 1, j + 1);
      *it++ = to_vertex_index(i, j + 1);
      *it++ = to_vertex_index(i, j);
      *it++ = to_vertex_index(i + 1, j);
      *it++ = to_vertex_index(i + 1, j + 1);
    }
  }

  return buffer;
}

} // namespace scene

} // namespace esim
