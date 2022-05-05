#include "surface_node.h"
#include "glapi/buffer_object.h"
#include "glapi/texture.h"
#include "transform/maptile.h"
#include "transform/wgs84.h"
#include <scene/programs/bounding_box_program.h>
#include <scene/programs/surface_program.h>
#include <vector>

namespace esim {

namespace scene {

namespace details {

class surface_vertex {
public:
  typedef surface_node_vertex vbo_buffer_type;
  typedef struct {
    double x, y, z;
    double tx, ty;
  }                           vertex_type;

  template <typename type>
  inline void push(type &&tilemap) noexcept {
    using namespace glm;
    assert(it_ != computing_.end());
    dvec3 temp;
    auto &vtx = *it_++;
    vtx.ty = tilemap.x;
    vtx.tx = tilemap.y;
    trans::maptile_to_geo(tilemap, temp);
    temp.z = 0;
    temp.x = glm::radians(temp.x);
    temp.y = glm::radians(temp.y);
    trans::wgs84geo_to_ecef(temp, temp);
    offset_ += temp;
    vtx.x = temp.x;
    vtx.y = temp.y;
    vtx.z = temp.z;

    max_.x = glm::max(max_.x, vtx.x);
    max_.y = glm::max(max_.y, vtx.y);
    max_.z = glm::max(max_.z, vtx.z);
    min_.x = glm::min(min_.x, vtx.x);
    min_.y = glm::min(min_.y, vtx.y);
    min_.z = glm::min(min_.z, vtx.z);
  }

  inline void confirm() noexcept {
    offset_ /= static_cast<double>(vertex_count_);
    max_ -= offset_;
    min_ -= offset_;
  }

  inline glm::dvec3 offset() const noexcept {

    return offset_;
  }

  inline std::vector<vbo_buffer_type> export_buffer() const noexcept {
    std::vector<vbo_buffer_type> res(vertex_count_);
    auto v_it = computing_.begin();
    for (auto &v : res) {
      auto &compute_v = *v_it++;
      v.x = static_cast<float>(compute_v.x - offset_.x);
      v.y = static_cast<float>(compute_v.y - offset_.y);
      v.z = static_cast<float>(compute_v.z - offset_.z);
      v.tx = static_cast<float>(compute_v.tx);
      v.ty = static_cast<float>(compute_v.ty);
    }

    return res;
  }

  inline std::vector<glm::vec3> export_bounding_box() const noexcept {
    std::vector<glm::vec3> res(8);
    auto v_it = res.begin();
    /// top
    (*v_it).x =   static_cast<float>(max_.x);
    (*v_it).y =   static_cast<float>(max_.y);
    (*v_it++).z = static_cast<float>(max_.z);
    (*v_it).x =   static_cast<float>(min_.x);
    (*v_it).y =   static_cast<float>(max_.y);
    (*v_it++).z = static_cast<float>(max_.z);
    (*v_it).x =   static_cast<float>(max_.x);
    (*v_it).y =   static_cast<float>(max_.y);
    (*v_it++).z = static_cast<float>(min_.z);
    (*v_it).x =   static_cast<float>(min_.x);
    (*v_it).y =   static_cast<float>(max_.y);
    (*v_it++).z = static_cast<float>(min_.z);
    
    // bottom
    (*v_it).x =   static_cast<float>(max_.x);
    (*v_it).y =   static_cast<float>(min_.y);
    (*v_it++).z = static_cast<float>(max_.z);
    (*v_it).x =   static_cast<float>(min_.x);
    (*v_it).y =   static_cast<float>(min_.y);
    (*v_it++).z = static_cast<float>(max_.z);
    (*v_it).x =   static_cast<float>(max_.x);
    (*v_it).y =   static_cast<float>(min_.y);
    (*v_it++).z = static_cast<float>(min_.z);
    (*v_it).x =   static_cast<float>(min_.x);
    (*v_it).y =   static_cast<float>(min_.y);
    (*v_it).z =   static_cast<float>(min_.z);

    return res;
  }

  surface_vertex(size_t vd) noexcept
      : vertex_count_{static_cast<uint32_t>((vd + 1) * (vd + 1))},
        computing_(vertex_count_),
        max_{std::numeric_limits<double>::min()},
        min_{std::numeric_limits<double>::max()},
        offset_{0.f},
        it_{computing_.begin()} {
  }

private:
  const uint32_t           vertex_count_;
  std::vector<vertex_type> computing_;
  glm::dvec3               max_, min_;
  glm::dvec3               offset_;
  std::vector<vertex_type>::iterator  it_;
};

} // namespace details

class surface_node::impl {
public:
  typedef surface_node_vertex vertex_type;
  typedef glm::vec3           bouding_vertex_type;

  inline void gen_vertex_buffer(size_t vd) noexcept {
    using namespace glm;
    const double tile_stride = 1.f / static_cast<double>(vd);
    const uint32_t zoom = node_details.zoom;
    const double xtile = node_details.xtile,
                 ytile = node_details.ytile;
    details::surface_vertex vertex_helper(vd);

    for (size_t i=0; i<=vd; ++i) {
      for (size_t j=0; j<=vd; ++j) {
        vertex_helper.push(dvec3{xtile + tile_stride * i, ytile + tile_stride * j, zoom});
      }
    }

    vertex_helper.confirm();
    vbo_.generate_buffer();
    vbo_.bind_buffer_data(vertex_helper.export_buffer());
    box_vbo_.generate_buffer();
    box_vbo_.bind_buffer_data(vertex_helper.export_bounding_box());
  }

  inline void draw([[maybe_unused]] const rendering_infos &info, size_t indices_count) noexcept {
    using namespace glm;
    auto &sun = info.sun;
    auto &cmr = info.camera;
    auto model = cmr.model(offset_);
    model = sun.rotate_to_solar_direction_on_equator(model);
    model = rotate(model,
                   static_cast<float>(trans::time::earth_rotation_angle(sun.julian_date())),
                   vec3{0.0f, 0.0f, 1.0f});
    // model = rotate(model, radians((static_cast<float>(timestamp % 86400) / 3600.0f * 15.0f)), vec3{0.0, 0.0, -1.0});

    auto program = surface_program::get();
    vbo_.bind();
    program->enable_position_pointer();
    program->enable_texture_coord_pointer();
    basemap_.bind();
    program->bind_model_uniform(model);
    program->bind_offset_uniform(vec3{0.5f});
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices_count), GL_UNSIGNED_SHORT, nullptr);
  }
  
  inline void draw_grid([[maybe_unused]] const rendering_infos &info, size_t indices_count) noexcept {
    using namespace glm;
    auto &cmr = info.camera;
    auto model = cmr.model(offset_);

    auto program = surface_program::get();
    vbo_.bind();
    program->enable_position_pointer();
    program->bind_model_uniform(model);
    program->bind_offset_uniform(vec3{1.0f, 1.0f, 1.0f});
    glPointSize(10);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(vbo_.size()));
    glDrawElements(GL_LINE_STRIP, static_cast<GLsizei>(indices_count), GL_UNSIGNED_SHORT, nullptr);
  }
  
  inline void draw_bounding_box([[maybe_unused]] const rendering_infos &info, size_t indices_count) noexcept {
    using namespace glm;
    auto &cmr = info.camera;
    auto model = cmr.model(offset_);
    
    auto program = bounding_box_program::get();
    box_vbo_.bind();
    program->enable_position_pointer();
    program->bind_model_uniform(model);
    glDepthFunc(GL_GREATER);
    program->bind_color_uniform(vec4{1.0f, 0.0f, 0.0f, 0.6f});
    glDrawElements(GL_LINES, static_cast<GLsizei>(indices_count), GL_UNSIGNED_SHORT, nullptr);
    glDepthFunc(GL_LESS);
    program->bind_color_uniform(vec4{0.0f, 1.0f, 0.0f, 0.6f});
    glDrawElements(GL_LINES, static_cast<GLsizei>(indices_count), GL_UNSIGNED_SHORT, nullptr);
  }

  impl(maptile &&tile) noexcept
      : node_details{tile}, offset_{0.f} {
    basemap_.generate();
    basemap_.bind_from_file("img/test_base000.jpg");
  }

private:
  inline static size_t to_index(const size_t &vd, size_t row, size_t col) noexcept {
    return row * vd + col;
  }

private:
  const maptile                          node_details;
  glm::dvec3                             offset_;
  gl::vertex_buffer<vertex_type>         vbo_;
  gl::vertex_buffer<bouding_vertex_type> box_vbo_;
  gl::texture                            basemap_;
};

void surface_node::draw(const rendering_infos &info, size_t indices_count) noexcept {
  if (nullptr != pimpl) {
    pimpl->draw(info, indices_count);
  }
}

void surface_node::draw_grid(const rendering_infos &info, size_t indices_count) noexcept {
  if (nullptr != pimpl) {
    pimpl->draw_grid(info, indices_count);
  }
}

void surface_node::draw_bounding_box(const rendering_infos &info, size_t indices_count) noexcept {
  if (nullptr != pimpl) {
    pimpl->draw_bounding_box(info, indices_count);
  }
}

void surface_node::gen_vertex(size_t vertex_details) noexcept {
  if (nullptr != pimpl) {
    pimpl->gen_vertex_buffer(vertex_details);
  }
}

surface_node::surface_node(maptile tile) noexcept
    : pimpl{make_ptr_u<impl>(std::move(tile))} {}

surface_node::~surface_node() noexcept {}

} // namespace scene

} // namespace esim
