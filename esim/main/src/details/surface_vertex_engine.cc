#include "surface_vertex_engine.h"
#include <cassert>
#include <functional>

namespace esim {

namespace details {

static size_t to_vertex_index(size_t i, size_t j, size_t vd) noexcept {

  return i * (vd + 3) + j;
}

static size_t to_center_vertex_index(size_t i, size_t j, size_t vd) noexcept {

  return i * (vd + 1) + j;
}

} // namespace details

const glm::dvec3 &surface_vertices::offset() const noexcept {

  return offset_;
}

const core::bounding_box &surface_vertices::obb() const noexcept {
  assert(nullptr != obb_);
  return *obb_;
}

double surface_vertices::tile_radius() const noexcept {

  return tile_radius_;
}

void surface_vertices::calculate() noexcept {
  using namespace glm;
  dvec3 north = {tile_.x + 0.5f, tile_.y, tile_.lod},
        south = {tile_.x + 0.5f, tile_.y + 1.0, tile_.lod};
  geo::maptile_to_geo(north, north); north.z = 0;
  geo::maptile_to_geo(south, south); south.z = 0;

  north = radians(north); north.z = 0;
  south = radians(south); south.z = 0;
  dvec3 middle = (north + south) / 2.0; 

  geo::geo_to_ecef(north, north);
  geo::geo_to_ecef(south, south);
  geo::geo_to_ecef(middle, middle);

  offset_ = middle;
  middle = normalize(middle);
  dvec3 basis = normalize(cross(north - south, middle));

  obb_ = make_uptr<core::bounding_box>(offset_, middle, basis);

  calculate_center();
  calculate_normal();
  calculate_skirt();
}

std::vector<surface_vertices::vbo_buffer_type> surface_vertices::export_buffer() const noexcept {
  using namespace glm;
  using namespace std::placeholders;
  auto to_index = std::bind(&details::to_vertex_index, _1, _2, vertex_details_);
  std::vector<vbo_buffer_type> output((vertex_details_ + 1) * (vertex_details_ + 1) +
                                      (vertex_details_ + 1) * 4);

  auto it = output.begin();
  for (size_t i = 0; i < vertex_details_ + 1; ++i) {
    for (size_t j = 0; j < vertex_details_ + 1; ++j) {
      auto &center = buffer_[to_index(i + 1, j + 1)];
      auto &vtx = *it++;
      vtx.pos = static_cast<vec3>(center.pos - offset_);
      vtx.normal = static_cast<vec3>(center.normal);
      vtx.texcoord = static_cast<vec2>(center.texcoord);
    }
  }

  auto skirt = buffer_.begin() + (vertex_details_ + 3) * (vertex_details_ + 3);
  while (skirt != buffer_.end()) {
    auto &skt = *skirt++;
    auto &vtx = *it++;
    vtx.pos = static_cast<vec3>(skt.pos - offset_);
    vtx.normal = static_cast<vec3>(skt.normal);
    vtx.texcoord = static_cast<vec2>(skt.texcoord);
  }

  return output;
}

std::vector<surface_vertices::obb_buffer_type> surface_vertices::export_obb_buffer() const noexcept {
  using namespace glm;
  std::vector<obb_buffer_type> res(8);
  auto &obb = obb_->data();
  auto v_it = res.begin();
  for (auto &v : obb) {
    (*v_it++).pos = static_cast<vec3>(v - offset_);
  }

  return res;
}

void surface_vertices::calculate_center() noexcept {
  using namespace glm;
  const double tile_stride = 1.0 / vertex_details_;
  const uint32_t lod = tile_.lod;
  /// contains a part of previous node to calculate normal vector
  const double xtile = tile_.x - tile_stride,
               ytile = tile_.y - tile_stride;
  buffer_.resize((vertex_details_ + 3) * (vertex_details_ + 3) + /// center buffer
                 (vertex_details_ + 1) * 4);                     /// skirt buffer
  auto it = buffer_.begin();
  for (size_t i = 0; i <= vertex_details_ + 2; ++i) {
    for (size_t j = 0; j <= vertex_details_ + 2; ++j) {
      assert(it != buffer_.end());
      auto &vtx = *it++;
      dvec3 curr{xtile + tile_stride * i, ytile + tile_stride * j, lod};

      vtx.texcoord.x = curr.y - tile_.y;
      vtx.texcoord.y = curr.x - tile_.x;

      geo::maptile_to_geo(curr, curr);
      curr = radians(curr); curr.z = 0;
      geo::geo_to_ecef(curr, vtx.pos);
    }
  }
}

void surface_vertices::calculate_skirt() noexcept {
  using namespace glm;
  using namespace std::placeholders;
  const double tile_stride = 1.0 / vertex_details_;
  const uint32_t lod = tile_.lod;
  auto north_skirt = buffer_.begin() + (vertex_details_ + 3) * (vertex_details_ + 3);
  auto west_skirt = north_skirt + (vertex_details_ + 1);
  auto east_skirt = west_skirt + (vertex_details_ + 1);
  auto south_skirt = east_skirt + (vertex_details_ + 1);
  auto to_index = std::bind(&details::to_vertex_index, _1, _2, vertex_details_);
  for (size_t i = 0; i < 1 + vertex_details_; ++i) {
    auto &n_vtx = *north_skirt++,
         &w_vtx = *west_skirt++,
         &e_vtx = *east_skirt++,
         &s_vtx = *south_skirt++;
    dvec3 nv{tile_.x                  , tile_.y + tile_stride * i, lod},
          wv{tile_.x + tile_stride * i, tile_.y                  , lod},
          ev{tile_.x + tile_stride * i, tile_.y + 1.0            , lod},
          sv{tile_.x + 1.0            , tile_.y + tile_stride * i, lod};

    geo::maptile_to_geo(nv, nv);
    geo::maptile_to_geo(wv, wv);
    geo::maptile_to_geo(ev, ev);
    geo::maptile_to_geo(sv, sv);

    if (tile_.x == 0) {
      nv.x = 90.0;
    }
    if (tile_.x == static_cast<uint32_t>((1 << tile_.lod) - 1)) {
      sv.x = -90.0;
    }

    nv = radians(nv); nv.z = - tile_radius_ * 0.01;
    wv = radians(wv); wv.z = - tile_radius_ * 0.01;
    ev = radians(ev); ev.z = - tile_radius_ * 0.01;
    sv = radians(sv); sv.z = - tile_radius_ * 0.01;

    n_vtx.pos = geo::geo_to_ecef(nv, nv);
    w_vtx.pos = geo::geo_to_ecef(wv, wv);
    e_vtx.pos = geo::geo_to_ecef(ev, ev);
    s_vtx.pos = geo::geo_to_ecef(sv, sv);

    if (tile_.x == 0) {
      obb_->update(n_vtx.pos);
    }
    if (tile_.x == static_cast<uint32_t>((1 << tile_.lod) - 1)) {
      obb_->update(s_vtx.pos);
    }

    auto &n_neighbor = buffer_[to_index(1                  , i + 1)],
         &w_neighbor = buffer_[to_index(i + 1              , 1)],
         &e_neighbor = buffer_[to_index(i + 1              , vertex_details_ + 1)],
         &s_neighbor = buffer_[to_index(vertex_details_ + 1, i + 1)];

    n_vtx.normal = n_neighbor.normal;
    w_vtx.normal = w_neighbor.normal;
    e_vtx.normal = e_neighbor.normal;
    s_vtx.normal = s_neighbor.normal;

    n_vtx.texcoord = n_neighbor.texcoord;
    w_vtx.texcoord = w_neighbor.texcoord;
    e_vtx.texcoord = e_neighbor.texcoord;
    s_vtx.texcoord = s_neighbor.texcoord;
  }

  obb_->calculate_box();
}

void surface_vertices::calculate_normal() noexcept {
  using namespace glm;
  using namespace std::placeholders;
  auto to_index = std::bind(&details::to_vertex_index, _1, _2, vertex_details_);
  for (size_t i = 1; i < vertex_details_ + 2; ++i) {
    for (size_t j = 1; j < vertex_details_ + 2; ++j) {
      auto &curr = buffer_[to_index(i, j)];
      obb_->update(curr.pos);
      tile_radius_ = max(tile_radius_, length(curr.pos - offset_));

      auto up = buffer_[to_index(i, j - 1)].pos - curr.pos;
      auto upleft = buffer_[to_index(i - 1, j - 1)].pos - curr.pos;
      auto left = buffer_[to_index(i - 1, j)].pos - curr.pos;
      auto downleft = buffer_[to_index(i - 1, j + 1)].pos - curr.pos;
      auto down = buffer_[to_index(i, j + 1)].pos - curr.pos;
      auto downright = buffer_[to_index(i + 1, j + 1)].pos - curr.pos;
      auto right = buffer_[to_index(i + 1, j)].pos - curr.pos;
      auto upright = buffer_[to_index(i + 1, j - 1)].pos - curr.pos;
      
      curr.normal  = cross(up, upleft);     curr.normal += cross(upright, up);    curr.normal += cross(right, upright);
      curr.normal += cross(upleft, left);                                         curr.normal += cross(downright, right);
      curr.normal += cross(left, downleft); curr.normal += cross(downleft, down); curr.normal += cross(down, downright);
      curr.normal /= 8.0;
      curr.normal = normalize(curr.normal);
    }
  }
}

surface_vertices::surface_vertices(const geo::maptile &tile, uint32_t details) noexcept
    : tile_{tile}, vertex_details_{details}, offset_{0.0},
      obb_(nullptr) {}

uptr<surface_vertices> surface_vertex_engine::gen_surface_vertices(const geo::maptile &tile) const noexcept {
  
  return make_uptr<surface_vertices>(tile, vertex_details_);
}

std::vector<uint16_t> surface_vertex_engine::export_center_element_buffer() const noexcept {
  using namespace std::placeholders;
  const uint32_t vert_count = static_cast<uint32_t>(vertex_details_ * vertex_details_);
  auto to_center_index = std::bind(&details::to_center_vertex_index, _1, _2, vertex_details_);
  std::vector<uint16_t> buffer(vert_count * 6);
  auto it = buffer.begin();
  for (size_t i = 0; i < vertex_details_; ++i) {
    for (size_t j = 0; j < vertex_details_; ++j) {
      *it++ = static_cast<uint16_t>(to_center_index(i, j));
      *it++ = static_cast<uint16_t>(to_center_index(i + 1, j + 1));
      *it++ = static_cast<uint16_t>(to_center_index(i, j + 1));
      *it++ = static_cast<uint16_t>(to_center_index(i, j));
      *it++ = static_cast<uint16_t>(to_center_index(i + 1, j));
      *it++ = static_cast<uint16_t>(to_center_index(i + 1, j + 1));
    }
  }

  return buffer;
}

std::vector<uint16_t> surface_vertex_engine::export_skirt_element_buffer() const noexcept {
  using namespace std::placeholders;
  std::vector<uint16_t> buffer(vertex_details_ * 6 * 4);
  auto to_index = std::bind(&details::to_center_vertex_index, _1, _2, vertex_details_);
  uint32_t skirt_offset = (vertex_details_ + 1) * (vertex_details_ + 1),
           skirt_stride = vertex_details_ + 1;
  auto n_it = buffer.begin(),
       w_it = n_it + (vertex_details_ * 6),
       e_it = w_it + (vertex_details_ * 6),
       s_it = e_it + (vertex_details_ * 6);

  for (size_t i = 0; i < vertex_details_; ++i) {
    /// north
    *n_it++ = static_cast<uint16_t>(skirt_offset + i);
    *n_it++ = static_cast<uint16_t>(to_index(0, i    ));
    *n_it++ = static_cast<uint16_t>(to_index(0, i + 1));
    *n_it++ = static_cast<uint16_t>(skirt_offset + i);
    *n_it++ = static_cast<uint16_t>(to_index(0, i + 1));
    *n_it++ = static_cast<uint16_t>(skirt_offset + i + 1);

    /// west
    *w_it++ = static_cast<uint16_t>(to_index(i + 0, 0));
    *w_it++ = static_cast<uint16_t>(skirt_offset + skirt_stride + i);
    *w_it++ = static_cast<uint16_t>(skirt_offset + skirt_stride + i + 1);
    *w_it++ = static_cast<uint16_t>(to_index(i + 0, 0));
    *w_it++ = static_cast<uint16_t>(skirt_offset + skirt_stride + i + 1);
    *w_it++ = static_cast<uint16_t>(to_index(i + 1, 0));
    
    /// east
    *e_it++ = static_cast<uint16_t>(to_index(i + 0, vertex_details_));
    *e_it++ = static_cast<uint16_t>(skirt_offset + skirt_stride + skirt_stride + i + 1);
    *e_it++ = static_cast<uint16_t>(skirt_offset + skirt_stride + skirt_stride + i);
    *e_it++ = static_cast<uint16_t>(to_index(i + 0, vertex_details_));
    *e_it++ = static_cast<uint16_t>(to_index(i + 1, vertex_details_));
    *e_it++ = static_cast<uint16_t>(skirt_offset + skirt_stride + skirt_stride + i + 1);

    /// south
    *s_it++ = static_cast<uint16_t>(skirt_offset + skirt_stride + skirt_stride + skirt_stride + i);
    *s_it++ = static_cast<uint16_t>(to_index(vertex_details_, i + 1));
    *s_it++ = static_cast<uint16_t>(to_index(vertex_details_, i + 0));
    *s_it++ = static_cast<uint16_t>(skirt_offset + skirt_stride + skirt_stride + skirt_stride + i);
    *s_it++ = static_cast<uint16_t>(skirt_offset + skirt_stride + skirt_stride + skirt_stride + i + 1);
    *s_it++ = static_cast<uint16_t>(to_index(vertex_details_, i + 1));
  }

  return buffer;
}

std::vector<uint16_t> surface_vertex_engine::export_obb_element_buffer() const noexcept {
  std::vector<uint16_t> buffer(24);
  auto it = buffer.begin();
  /// box top
  *it++ = 0; *it++ = 1;
  *it++ = 1; *it++ = 3;
  *it++ = 3; *it++ = 2;
  *it++ = 2; *it++ = 0;

  /// box bottom
  *it++ = 4; *it++ = 5;
  *it++ = 5; *it++ = 7;
  *it++ = 7; *it++ = 6;
  *it++ = 6; *it++ = 4;

  /// box side
  *it++ = 0; *it++ = 4;
  *it++ = 1; *it++ = 5;
  *it++ = 2; *it++ = 6;
  *it++ = 3; *it++ = 7;

  return buffer;
}

surface_vertex_engine::surface_vertex_engine(uint32_t details) noexcept
    : vertex_details_{details} {}

} // namespace esim
