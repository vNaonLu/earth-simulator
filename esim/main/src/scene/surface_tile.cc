#include "scene/surface_tile.h"
#include "core/bounding_box.h"
#include <cstdint>
#include <glad/glad.h>

namespace esim {

namespace details {

class surface_tile_helper {
public:
  typedef surface_vertex      vbo_buffer_type;
  typedef bounding_box_vertex obb_buffer_type;
  typedef struct {
    glm::dvec3 pos;
    glm::dvec3 normal;
    glm::dvec2 texcoord;
  } vertex_type;

  template <typename type>
  void push(type &&tilemap) noexcept;

  void calculate_normal() noexcept;

  glm::dvec3 offset() const noexcept;

  std::vector<vbo_buffer_type> export_buffer() noexcept;

  std::vector<obb_buffer_type> export_bounding_box_vertex() const noexcept;

  core::bounding_box export_bounding_box() const noexcept;

  surface_tile_helper(const geo::maptile &tile, size_t vd, glm::dvec3 offset, glm::dvec3 basis) noexcept;

  double bounding_radius() const noexcept;

private:
  size_t to_index(size_t i, size_t j) const noexcept;

  size_t to_buffer_index(size_t i, size_t j) const noexcept;

private:
  const geo::maptile       tile_;
  const uint32_t           vertex_details_;
  const uint32_t           vertex_count_;
  const glm::dvec3         offset_;
  double                   radius_;
  std::vector<vertex_type> computing_;
  std::vector<vertex_type>::iterator  it_;

  core::bounding_box       bounding_box_;
};

template <typename type>
void surface_tile_helper::push(type &&tilemap) noexcept {
  using namespace glm;
  assert(it_ != computing_.end());
  dvec3 temp;
  auto &vtx = *it_++;
  vtx.texcoord.y = tilemap.x - tile_.x;
  vtx.texcoord.x = tilemap.y - tile_.y;
  geo::maptile_to_geo(tilemap, temp);
  temp.z = 0;
  temp.x = radians(temp.x);
  temp.y = radians(temp.y);
  geo::geo_to_ecef(temp, temp);
  vtx.pos = temp;

  bounding_box_.update(vtx.pos);
}

void surface_tile_helper::calculate_normal() noexcept {
  using namespace glm;
  /// calculating normal
  bounding_box_.calculate_box();
  for (size_t i = 1; i < vertex_details_ + 2; ++i) {
    for (size_t j = 1; j < vertex_details_ + 2; ++j) {
      auto &curr = computing_[to_index(i, j)];
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
}

glm::dvec3 surface_tile_helper::offset() const noexcept {

  return offset_;
}

std::vector<surface_tile_helper::vbo_buffer_type> surface_tile_helper::export_buffer() noexcept {
  using namespace glm;
  std::vector<vbo_buffer_type> res((vertex_details_ + 1) * (vertex_details_ + 1));
  auto v_it = computing_.begin();
  for (size_t i = 0; i < vertex_details_ + 1; ++i) {
    for (size_t j = 0; j < vertex_details_ + 1; ++j) {
      auto &compute_v = computing_[to_index(i + 1, j + 1)];
      auto &v = res[to_buffer_index(i, j)];
      radius_ = max(radius_, length(compute_v.pos - offset_));
      v.pos = static_cast<vec3>(compute_v.pos - offset_);
      v.normal = static_cast<vec3>(compute_v.normal);
      v.texcoord = static_cast<vec2>(compute_v.texcoord);
    }
  }

  return res;
}

std::vector<surface_tile_helper::obb_buffer_type> surface_tile_helper::export_bounding_box_vertex() const noexcept {
  using namespace glm;
  std::vector<obb_buffer_type> res(8);
  auto &obb = bounding_box_.data();
  auto v_it = res.begin();

  for (auto &v : obb) {
    (*v_it++).pos = static_cast<vec3>(v - offset_);
  }

  return res;
}

core::bounding_box surface_tile_helper::export_bounding_box() const noexcept {
  
  return bounding_box_;
}

surface_tile_helper::surface_tile_helper(const geo::maptile &tile,
                                         size_t vd,
                                         glm::dvec3 offset,
                                         glm::dvec3 basis) noexcept
    : tile_{tile}, vertex_details_{static_cast<uint32_t>(vd)},
      vertex_count_{static_cast<uint32_t>((vd + 3) * (vd + 3))},
      offset_{offset}, radius_{0.0}, computing_(vertex_count_),
      it_{computing_.begin()}, bounding_box_{offset, offset, basis} {
}

double surface_tile_helper::bounding_radius() const noexcept {

  return radius_;
}

size_t surface_tile_helper::to_index(size_t i, size_t j) const noexcept {

  return i * (vertex_details_ + 3) + j;
}

size_t surface_tile_helper::to_buffer_index(size_t i, size_t j) const noexcept {

  return i * (vertex_details_ + 1) + j;
}

} // namespace details

namespace scene {

const geo::maptile &surface_tile::details() const noexcept {

  return info_;
}

void surface_tile::gen_vertex_buffer(size_t details) noexcept {
  using namespace glm;
  const double tile_stride = 1.f / static_cast<double>(details);
  const uint32_t zoom = info_.lod;
  /// with previous node to calculate normal vector
  const double xtile = info_.x - tile_stride,
               ytile = info_.y - tile_stride;
  dvec3 center = {xtile + 0.5f, ytile + 0.5f, zoom},
        north = {xtile + 0.5f, ytile, zoom};
  geo::maptile_to_geo(center, center); center.z = 0;
  geo::maptile_to_geo(north, north); north.z = 0;
  center = radians(center); north = radians(north);
  geo::geo_to_ecef(center, center); geo::geo_to_ecef(north, north);
  north = north - center;
  /// adjust to 90 degree
  dvec3 basis = normalize(cross(center, north));
  details::surface_tile_helper vertex_helper(info_, details, center, basis);

  for (size_t i = 0; i <= details + 2; ++i) {
    for (size_t j = 0; j <= details + 2; ++j) {
      vertex_helper.push(dvec3{xtile + tile_stride * i, ytile + tile_stride * j, zoom});
    }
  }

  vertex_helper.calculate_normal();
  offset_ = vertex_helper.offset();
  temp_vertex_ = vertex_helper.export_buffer();
  temp_obb_vertex_ = vertex_helper.export_bounding_box_vertex();
  bounding_box_ = vertex_helper.export_bounding_box();
  terrain_radius_ = vertex_helper.bounding_radius();

  ready_to_render_ = true;
}

bool surface_tile::is_ready_to_render() const noexcept {

  return ready_to_render_;
}

void surface_tile::before_render() noexcept {
  if (!buffer_generated_) {
    vbo_ = make_uptr<gl::buffer<details::surface_vertex>>(GL_ARRAY_BUFFER);
    vbo_->bind_buffer(std::move(temp_vertex_));

    obb_vbo_ = make_uptr<gl::buffer<details::bounding_box_vertex>>(GL_ARRAY_BUFFER);
    obb_vbo_->bind_buffer(std::move(temp_obb_vertex_));

    buffer_generated_ = true;
  }
}

void surface_tile::render(const scene::frame_info &info,
                          size_t indices_count) noexcept {
  using namespace glm;
  before_render();
  auto &sun = info.sun;
  auto &cmr = info.camera;
  auto model = rotate(dmat4x4{1.0f}, astron::era<double>(sun.julian_date()), dvec3{0.0f, 0.0f, 1.0f});
  model = cmr.translate(dmat4x4{1.0f}, offset_);
  auto program = program::surface_program::get();
  // basemap_.bind();
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
  auto model = rotate(dmat4x4{1.0f}, astron::era<double>(sun.julian_date()), dvec3{0.0f, 0.0f, 1.0f});
  model = cmr.translate(dmat4x4{1.0f}, offset_);
  auto program = program::bounding_box_program::get();
  obb_vbo_->bind();
  program->enable_position_pointer();
  program->update_model_uniform(static_cast<mat4x4>(model));
  glDrawElements(GL_LINES, static_cast<GLsizei>(indices_count), GL_UNSIGNED_SHORT, nullptr);
}

surface_tile::surface_tile(geo::maptile tile) noexcept
    : info_{tile}, ready_to_render_{false}, buffer_generated_{false},
      terrain_radius_{0.0}, offset_{0.0f},
      bounding_box_{offset_}, parent_{nullptr} {
  // assert(basemap_.load("assets/img/test_base000.jpg"));
}

std::pair<bool, bool>
surface_tile::is_enough_resolution(const scene::frame_info &info) const noexcept {
  using namespace glm;
  std::pair<bool, bool> resolution_check{false, false};
  const auto &cmr = info.camera;
  auto P_matrix = cmr.project<float>();
  auto NDC = P_matrix * vec4{static_cast<float>(2.0 * terrain_radius_),
                             static_cast<float>(2.0 * terrain_radius_),
                             static_cast<float>(length(-offset_ + cmr.pos())), 1.0f};
  NDC /= NDC.w;
  NDC = abs(NDC);

  auto &[too_far, too_near] = resolution_check;
  too_near = info_.lod < 2 && ((NDC.x > 2.0f) || (NDC.y > 2.0f));
  too_far = info_.lod > 0 && ((NDC.x < 0.5f) || (NDC.y < 0.5f));

  return resolution_check;
}

bool surface_tile::is_visible(const scene::frame_info &info) const noexcept {
  /// reference: https://cesium.com/blog/2013/04/25/horizon-culling/
  using namespace glm;
  constexpr static dvec3 base = {geo::wgs84::A, geo::wgs84::A, geo::wgs84::B};
  dvec3 cv = info.camera.pos() / base;
  double vh_magnitude_sq = dot(cv, cv) - 1.0f;

  bool is_occluded = true;
  for (auto &v : bounding_box_.data()) {
    dvec3 pt = (v + offset_) / base,
          vt = pt - cv;
    double vt_magnitude_sq = dot(vt, vt);
    double vt_dot_vc = -dot(cv, vt);

    is_occluded &= vt_dot_vc > vh_magnitude_sq &&
                   vt_dot_vc * vt_dot_vc / vt_magnitude_sq > vh_magnitude_sq;
  }

  return !is_occluded;
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
