#include "scene/surface_tile.h"
#include <cstdint>
#include <glad/glad.h>

namespace esim {

namespace details {

class surface_tile_helper {
public:
  typedef surface_vertex vbo_buffer_type;
  typedef struct {
    glm::dvec3 pos;
    glm::dvec3 normal;
    glm::dvec2 texcoord;
  } vertex_type;

  template <typename type>
  void push(type &&tilemap) noexcept;

  void confirm() noexcept;

  glm::dvec3 offset() const noexcept;

  std::vector<vbo_buffer_type> export_buffer() const noexcept;

  std::vector<glm::vec3> export_bounding_box() const noexcept;

  surface_tile_helper(size_t vd) noexcept;

private:
  size_t to_index(size_t i, size_t j) const noexcept;

  size_t to_buffer_index(size_t i, size_t j) const noexcept;

private:
  const uint32_t           vertex_details_;
  const uint32_t           vertex_count_;
  std::vector<vertex_type> computing_;
  glm::dvec3               max_, min_;
  glm::dvec3               offset_;
  std::vector<vertex_type>::iterator  it_;
};

template <typename type>
void surface_tile_helper::push(type &&tilemap) noexcept {
  using namespace glm;
  assert(it_ != computing_.end());
  dvec3 temp;
  auto &vtx = *it_++;
  vtx.texcoord.y = tilemap.x;
  vtx.texcoord.x = tilemap.y;
  geo::maptile_to_geo(tilemap, temp);
  temp.z = 0;
  temp.x = radians(temp.x);
  temp.y = radians(temp.y);
  geo::geo_to_ecef(temp, temp);
  vtx.pos = temp;

  max_.x = max(max_.x, vtx.pos.x);
  max_.y = max(max_.y, vtx.pos.y);
  max_.z = max(max_.z, vtx.pos.z);
  min_.x = min(min_.x, vtx.pos.x);
  min_.y = min(min_.y, vtx.pos.y);
  min_.z = min(min_.z, vtx.pos.z);
}

void surface_tile_helper::confirm() noexcept {
  using namespace glm;
  /// calculating normal
  for (size_t i = 1; i < vertex_details_ + 2; ++i) {
    for (size_t j = 1; j < vertex_details_ + 2; ++j) {
      auto &curr = computing_[to_index(i, j)];
      if (j != vertex_details_ - 1) {
        offset_ += curr.pos;
      }
      auto up = computing_[to_index(i, j - 1)].pos - curr.pos;
      auto upleft = computing_[to_index(i - 1, j - 1)].pos - curr.pos;
      auto left = computing_[to_index(i - 1, j)].pos - curr.pos;
      auto downleft = computing_[to_index(i - 1, j + 1)].pos - curr.pos;
      auto down = computing_[to_index(i, j + 1)].pos - curr.pos;
      auto downright = computing_[to_index(i + 1, j + 1)].pos - curr.pos;
      auto right = computing_[to_index(i + 1, j)].pos - curr.pos;
      auto upright = computing_[to_index(i + 1, j - 1)].pos - curr.pos;
      curr.normal  = cross(up, upleft);
      curr.normal += cross(upleft, left);
      curr.normal += cross(left, downleft);
      curr.normal += cross(downleft, down);
      curr.normal += cross(down, downright);
      curr.normal += cross(downright, right);
      curr.normal += cross(right, upright);
      curr.normal += cross(upright, up);
      curr.normal /= 8.0;
      curr.normal = normalize(curr.normal);
    }
  }
  offset_ /= static_cast<double>(vertex_count_);
  max_ -= offset_;
  min_ -= offset_;
}

glm::dvec3 surface_tile_helper::offset() const noexcept {

  return offset_;
}

std::vector<surface_tile_helper::vbo_buffer_type> surface_tile_helper::export_buffer() const noexcept {
  std::vector<vbo_buffer_type> res((vertex_details_ + 1) * (vertex_details_ + 1));
  auto v_it = computing_.begin();
  for (size_t i = 0; i < vertex_details_ + 1; ++i) {
    for (size_t j = 0; j < vertex_details_ + 1; ++j) {
      auto &compute_v = computing_[to_index(i + 1, j + 1)];
      auto &v = res[to_buffer_index(i, j)];
      v.pos.x = static_cast<float>(compute_v.pos.x - offset_.x);
      v.pos.y = static_cast<float>(compute_v.pos.y - offset_.y);
      v.pos.z = static_cast<float>(compute_v.pos.z - offset_.z);
      v.normal.x = static_cast<float>(compute_v.normal.x);
      v.normal.y = static_cast<float>(compute_v.normal.y);
      v.normal.z = static_cast<float>(compute_v.normal.z);
      v.texcoord.x = static_cast<float>(compute_v.texcoord.x);
      v.texcoord.y = static_cast<float>(compute_v.texcoord.y);
    }
  }

  return res;
}

std::vector<glm::vec3> surface_tile_helper::export_bounding_box() const noexcept {
  std::vector<glm::vec3> res(8);
  auto v_it = res.begin();
  /// top
  (*v_it).x = static_cast<float>(max_.x);
  (*v_it).y = static_cast<float>(max_.y);
  (*v_it++).z = static_cast<float>(max_.z);
  (*v_it).x = static_cast<float>(min_.x);
  (*v_it).y = static_cast<float>(max_.y);
  (*v_it++).z = static_cast<float>(max_.z);
  (*v_it).x = static_cast<float>(max_.x);
  (*v_it).y = static_cast<float>(max_.y);
  (*v_it++).z = static_cast<float>(min_.z);
  (*v_it).x = static_cast<float>(min_.x);
  (*v_it).y = static_cast<float>(max_.y);
  (*v_it++).z = static_cast<float>(min_.z);

  // bottom
  (*v_it).x = static_cast<float>(max_.x);
  (*v_it).y = static_cast<float>(min_.y);
  (*v_it++).z = static_cast<float>(max_.z);
  (*v_it).x = static_cast<float>(min_.x);
  (*v_it).y = static_cast<float>(min_.y);
  (*v_it++).z = static_cast<float>(max_.z);
  (*v_it).x = static_cast<float>(max_.x);
  (*v_it).y = static_cast<float>(min_.y);
  (*v_it++).z = static_cast<float>(min_.z);
  (*v_it).x = static_cast<float>(min_.x);
  (*v_it).y = static_cast<float>(min_.y);
  (*v_it).z = static_cast<float>(min_.z);

  return res;
}

surface_tile_helper::surface_tile_helper(size_t vd) noexcept
    : vertex_details_{static_cast<uint32_t>(vd)},
      vertex_count_{static_cast<uint32_t>((vd + 3) * (vd + 3))},
      computing_(vertex_count_),
      max_{std::numeric_limits<double>::min()},
      min_{std::numeric_limits<double>::max()},
      offset_{0.f},
      it_{computing_.begin()} {
}

size_t surface_tile_helper::to_index(size_t i, size_t j) const noexcept {

  return i * (vertex_details_ + 3) + j;
}

size_t surface_tile_helper::to_buffer_index(size_t i, size_t j) const noexcept {

  return i * (vertex_details_ + 1) + j;
}

} // namespace details

namespace scene {

void surface_tile::gen_vertex_buffer(size_t details) noexcept {
  using namespace glm;
  const double tile_stride = 1.f / static_cast<double>(details);
  const uint32_t zoom = info_.lod;
  /// with previous node to calculate normal verctor
  const double xtile = info_.x - tile_stride,
               ytile = info_.y - tile_stride;
  details::surface_tile_helper vertex_helper(details);

  for (size_t i = 0; i <= details + 2; ++i) {
    for (size_t j = 0; j <= details + 2; ++j) {
      vertex_helper.push(dvec3{xtile + tile_stride * i, ytile + tile_stride * j, zoom});
    }
  }

  vertex_helper.confirm();
  offset_ = vertex_helper.offset();
  vbo_.bind_buffer(vertex_helper.export_buffer());
  // box_vbo_.generate_buffer();
  // box_vbo_.bind_buffer_data(vertex_helper.export_bounding_box());
}

void surface_tile::render(const scene::frame_info &info,
                          size_t indices_count) noexcept {
  using namespace glm;
  auto &sun = info.sun;
  auto &cmr = info.camera;
  auto model = static_cast<mat4x4>(cmr.translate(dmat4x4{1.0f}, offset_));
  model = rotate(model, astron::era<float>(sun.julian_date()),
                 vec3{0.0f, 0.0f, 1.0f});
  auto program = program::surface_program::get();
  basemap_.bind();
  vbo_.bind();
  program->enable_position_pointer();
  program->enable_normal_pointer();
  program->enable_texcoord_pointer();
  program->update_model_uniform(model);
  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices_count), GL_UNSIGNED_SHORT, nullptr);
}

// void draw_bounding_box([[maybe_unused]] const rendering_infos &info, size_t indices_count) noexcept {
//   using namespace glm;
//   auto &cmr = info.camera;
//   auto model = cmr.model(offset_);
  
//   auto program = bounding_box_program::get();
//   box_vbo_.bind();
//   program->enable_position_pointer();
//   program->bind_model_uniform(model);
//   glDepthFunc(GL_GREATER);
//   program->bind_color_uniform(vec4{1.0f, 0.0f, 0.0f, 0.6f});
//   glDrawElements(GL_LINES, static_cast<GLsizei>(indices_count), GL_UNSIGNED_SHORT, nullptr);
//   glDepthFunc(GL_LESS);
//   program->bind_color_uniform(vec4{0.0f, 1.0f, 0.0f, 0.6f});
//   glDrawElements(GL_LINES, static_cast<GLsizei>(indices_count), GL_UNSIGNED_SHORT, nullptr);
// }

surface_tile::surface_tile(geo::maptile tile) noexcept
    : info_{tile}, offset_{0.0f}, vbo_{GL_ARRAY_BUFFER} {
  assert(basemap_.load("assets/img/test_base000.jpg"));
}

} // namespace scene

} // namespace esim
