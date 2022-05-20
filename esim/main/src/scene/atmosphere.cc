#include "scene/atmosphere.h"
#include "core/transform.h"

namespace esim {

namespace scene {

void atmosphere::render(const frame_info &info) noexcept {
  using namespace glm;
  auto &cmr = info.camera;
  auto program = program::atmosphere_program::get();
  program->use();
  ebo_.bind();
  vbo_.bind();
  program->enable_position_pointer();
  program->update_common_uniform(info);
  program->update_model_uniform(cmr.translate<float>(mat4{1.0f}, offset_));
  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(ebo_.size()), GL_UNSIGNED_INT, nullptr);
}

atmosphere::atmosphere() noexcept
    : offset_{0.0f},
      ebo_{GL_ELEMENT_ARRAY_BUFFER, 1},
      vbo_{GL_ARRAY_BUFFER, 1} {
  vbo_.bind_buffer(gen_vertex_buffer());
  ebo_.bind_buffer(gen_element_buffer());
}

atmosphere::~atmosphere() noexcept {}

namespace details {
constexpr static uint32_t latitude_details = 180;
constexpr static uint32_t longitude_details = 360;
constexpr static double max_latitude = +90.0;
constexpr static double min_latitude = -90.0;
constexpr static double stride_latitude = (max_latitude - min_latitude) / latitude_details;
constexpr static double max_longitude = +180.0;
constexpr static double min_longitude = -180.0;
constexpr static double stride_longitude = (max_longitude - min_longitude) / longitude_details;
} // namespace details

uint32_t to_vertex_index(uint32_t row, uint32_t col) noexcept {
  return row * (details::longitude_details) + col;
}

std::vector<esim::details::atmosphere_vertex> atmosphere::gen_vertex_buffer() noexcept {
  using namespace glm;
  std::vector<esim::details::atmosphere_vertex> buffer((details::latitude_details + 1) *
                                                       (details::longitude_details + 1));
  auto it = buffer.begin();
  for (size_t i = 0; i <= details::latitude_details; ++i) {
    for (size_t j = 0; j <= details::longitude_details; ++j) {
      auto &v = (*it++).pos;
      dvec3 ecef{radians(details::min_latitude + details::stride_latitude * i),
                 radians(details::min_longitude + details::stride_longitude * j),
                 100'000};
      geo::geo_to_ecef(ecef, ecef);
      v.x = static_cast<float>(ecef.x);
      v.y = static_cast<float>(ecef.y);
      v.z = static_cast<float>(ecef.z);
      if (j != details::longitude_details) {
        /// avoid duplicatly computing same point
        offset_ += v;
      }
    }
  }

  offset_ /= details::latitude_details * details::longitude_details;

  for (auto &v : buffer) {
    v.pos -= static_cast<vec3>(offset_);
  }

  return buffer;
}

std::vector<uint32_t> atmosphere::gen_element_buffer() noexcept {
  using namespace glm;
  std::vector<uint32_t> buffer((details::latitude_details + 1) *
                               details::longitude_details * 6);
  auto it = buffer.begin();
  for (uint32_t i = 0; i < details::latitude_details; ++i) {
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
