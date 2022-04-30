#include "surface_node.h"
#include "coord/transform.h"
#include "glapi/buffer_object.h"
#include "surface_program.h"
#include <glfw/glfw3.h>
#include <vector>

namespace esim {

namespace scene {

class surface_node::impl {
public:
  typedef surface_node_vertex vertex_type;

  inline void gen_vertex_buffer(size_t vd) noexcept {
    using namespace glm;
    const uint32_t vert_count = (vd + 1) * (vd + 1);
    const double tile_stride = 1.L / static_cast<double>(vd);
    const uint32_t zoom = node_details.zoom;
    const double xtile = node_details.xtile,
                 ytile = node_details.ytile;
    double &offx = offset_.x = 0.f,
           &offy = offset_.y = 0.f,
           &offz = offset_.z = 0.f;
    std::vector<dvec3>       computing(vert_count, dvec3{0.f});
    std::vector<vertex_type> buffer(vert_count);

    for (size_t i=0; i<=vd; ++i) {
      for (size_t j=0; j<=vd; ++j) {
        auto &computing_buffer = computing[to_index(vd, i, j)];
        auto &true_buffer = buffer[to_index(vd, i, j)];
        true_buffer.lat = static_cast<float>(
            computing_buffer.x = coord::tiley_to_lati(zoom, ytile + tile_stride * i));
        true_buffer.lon = static_cast<float>(
            computing_buffer.y = coord::tilex_to_long(zoom, xtile + tile_stride * j));
        computing_buffer.z = 0.0;
        coord::geodetic_to_ecef(computing_buffer.x, computing_buffer.y, computing_buffer.z);
        offset_.x += computing_buffer.x;
        offset_.y += computing_buffer.y;
        offset_.z += computing_buffer.z;
      }
    }

    offset_ /= vert_count;
    
    auto v_it = computing.begin();
    for (auto &v : buffer) {
      auto &compute_v = *v_it++;
      v.x = (float) compute_v.x - offset_.x;
      v.y = (float) compute_v.y - offset_.y;
      v.z = (float) compute_v.z - offset_.z;
    }

    vbo_.generate_buffer();
    vbo_.bind_buffer_data(buffer);
  }

  inline void draw(const camera &cmr, size_t indices_count) noexcept {
    using namespace glm;
    auto m = rotate(mat4x4{1.0f}, radians((float)glfwGetTime() * 5), vec3(0.f, 1.f, 1.f));

    auto program = surface_program::get();
    vbo_.bind();
    program->enable_position_pointer();
    program->bind_model_uniform(m);
    glPointSize(10);
    program->bind_offset_uniform(vec3{0.5f});
    glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_SHORT, nullptr);
    program->bind_offset_uniform(vec3{1.0f, 1.0f, 1.0f});
    glDrawArrays(GL_POINTS, 0, vbo_.size());
    glDrawElements(GL_LINE_STRIP, indices_count, GL_UNSIGNED_SHORT, nullptr);
  }

  impl(maptile &&tile) noexcept
      : node_details{tile}, offset_{0.f} {
  }

private:
  inline static size_t to_index(const size_t &vd, size_t row, size_t col) noexcept {
    return row * vd + col;
  }

private:
  const maptile node_details;
  gl::vertex_buffer<vertex_type> vbo_;
  glm::dvec3 offset_;
};

void surface_node::draw(const camera &cmr, size_t indices_count) noexcept {
  if (nullptr != pimpl) {
    pimpl->draw(cmr, indices_count);
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
