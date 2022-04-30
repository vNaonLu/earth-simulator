#include "surface_layer.h"
#include "glapi/buffer_object.h"
#include "surface_node.h"
#include "surface_program.h"
#include <vector>

namespace esim {

namespace scene {

class surface_layer::impl {
public:
  inline void gen_indices_buffer() noexcept {
    const uint32_t vert_count = vertex_details * vertex_details;
    std::vector<uint16_t> buffer(vert_count * 6);
    auto it = buffer.begin();
    for (size_t i = 0; i < vertex_details; ++i) {
      for (size_t j = 0; j < vertex_details; ++j) {
        *it++ = to_vertex_index(i  , j  );
        *it++ = to_vertex_index(i+1, j+1);
        *it++ = to_vertex_index(i  , j+1);
        *it++ = to_vertex_index(i  , j  );
        *it++ = to_vertex_index(i+1, j  );
        *it++ = to_vertex_index(i+1, j+1);
      }
    }

    ibo_.generate_buffer();
    ibo_.bind_buffer_data(buffer);
  }

  inline void draw(const camera &cmr) noexcept {
    using namespace glm;
    auto program = surface_program::get();
    program->use();
    program->bind_vp_uniform(cmr.get_vp());
    ibo_.bind();
    for (auto &node : drawings) {
      node->draw(cmr, ibo_.size());
    }
  }

  impl(size_t vd) noexcept
      : vertex_details{vd} {
    gen_indices_buffer();
    drawings.emplace_back(make_ptr_u<surface_node>(maptile{0, 0, 0}));
    drawings.front()->gen_vertex(vertex_details);
  }

private:
  inline size_t to_vertex_index(size_t row, size_t col) const noexcept {
    return row * (vertex_details) + col;
  }

private:
  size_t vertex_details;
  gl::index_buffer<uint16_t> ibo_;
  std::vector<u_ptr<surface_node>> drawings;
};

void surface_layer::draw(const camera &cmr) noexcept {
  if (nullptr != pimpl) {
    pimpl->draw(cmr);
  }
}

surface_layer::surface_layer(size_t node_details) noexcept
    : pimpl{make_ptr_u<impl>(node_details)} {}

surface_layer::~surface_layer() noexcept {}

} // namespace cfene

} // namespace esim
