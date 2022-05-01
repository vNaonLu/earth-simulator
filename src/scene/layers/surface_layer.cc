#include "surface_layer.h"
#include "glapi/buffer_object.h"
#include "surface_node.h"
#include <scene/programs/bounding_box_program.h>
#include <scene/programs/surface_program.h>
#include <vector>

namespace esim {

namespace scene {

class surface_layer::impl {
public:
  inline void gen_indices_buffer() noexcept {
    const uint32_t vert_count = static_cast<uint32_t>(vertex_details * vertex_details);
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

    ibo_.bind_buffer_data(std::move(buffer), GL_STATIC_DRAW, 0);
  }
  
  inline void gen_bounding_box_indices_buffer() noexcept {
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

    ibo_.bind_buffer_data(std::move(buffer), GL_STATIC_DRAW, 1);
  }

  inline void draw(const camera &cmr) noexcept {
    using namespace glm;
    auto program = surface_program::get();
    ibo_.bind(0);
    program->use();
    program->bind_view_uniform(cmr.view());
    program->bind_proj_uniform(cmr.projection());
    for (auto &node : drawings) {
      node->draw(cmr, ibo_.size());
      node->draw_grid(cmr, ibo_.size(0));
    }
  }
  
  inline void draw_bounding_box(const camera &cmr) noexcept {
    using namespace glm;
    auto program = bounding_box_program::get();
    ibo_.bind(1);
    program->use();
    program->bind_view_uniform(cmr.view());
    program->bind_proj_uniform(cmr.projection());
    for (auto &node : drawings) {
      node->draw_bounding_box(cmr, ibo_.size(1));
    }
  }

  impl(size_t vd) noexcept
      : vertex_details{vd} {
    ibo_.generate_buffer(2);
    gen_indices_buffer();
    gen_bounding_box_indices_buffer();

    /// TODO: node tree
    drawings.emplace_back(make_ptr_u<surface_node>(maptile{0, 0, 0}));
    drawings.front()->gen_vertex(vertex_details);
  }

private:
  inline uint16_t to_vertex_index(size_t row, size_t col) const noexcept {
    return static_cast<uint16_t>(row * (vertex_details) + col);
  }

private:
  size_t vertex_details;
  /// 0 stores normal indices, and 1 stores bounding box.
  gl::index_buffer<uint16_t>       ibo_;
  std::vector<u_ptr<surface_node>> drawings;
};

void surface_layer::draw(const camera &cmr) noexcept {
  if (nullptr != pimpl) {
    pimpl->draw(cmr);
  }
}

void surface_layer::draw_bounding_box(const camera &cmr) noexcept {
  if (nullptr != pimpl) {
    pimpl->draw_bounding_box(cmr);
  }
}

surface_layer::surface_layer(size_t node_details) noexcept
    : pimpl{make_ptr_u<impl>(node_details)} {}

surface_layer::~surface_layer() noexcept {}

} // namespace cfene

} // namespace esim
