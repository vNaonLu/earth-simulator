#include "scene/surface_collections.h"
#include "scene/programs/bounding_box_program.h"

namespace esim {

namespace scene {

void surface_collection::render(const scene::frame_info &info) noexcept {
  auto program = program::surface_program::get();
  program->use();
  ebo_.bind(0);
  program->update_common_uniform(info);

  updating_queue_.try_push(info);

  if (next_frame_prepared_.load(std::memory_order_acquire)) {
    render_tiles_.swap(next_frame_tiles_);
    next_frame_prepared_.store(false, std::memory_order_release);
  }

  for (auto &node : render_tiles_) {
    node->render(info, ebo_.size(0));
  }
}

void surface_collection::render_bounding_box([[maybe_unused]] const scene::frame_info &info) noexcept {
  using namespace glm;
  auto program = program::bounding_box_program::get();
  ebo_.bind(1);
  program->use();
  program->update_common_uniform(info);
  program->update_line_color_uniform(vec4{0.0, 1.0, 0.0, 0.8});
  for (auto &node : render_tiles_) {
    node->render_bounding_box(info, ebo_.size(1));
  }
}

surface_collection::surface_collection(size_t vertex_details) noexcept
    : vertex_details_{vertex_details}, ebo_{GL_ELEMENT_ARRAY_BUFFER, 2},
      next_frame_prepared_{false}, is_working_{false},
      surface_root_{make_uptr<surface_tile>(geo::maptile{0, 0, 0})},
      updating_queue_{256} {
  ebo_.bind_buffer(gen_surface_element_buffer(), GL_STATIC_DRAW, 0);
  ebo_.bind_buffer(gen_bounding_box_element_buffer(), GL_STATIC_DRAW, 1);
  candidate_tiles_.emplace(surface_root_.get());
  is_working_.store(true, std::memory_order_release);

  std::thread([=]() {
    while (is_working_.load(std::memory_order_acquire)) {
      this->prepare_render();
    }
  }).detach();
}

surface_collection::~surface_collection() noexcept {
  is_working_.store(false, std::memory_order_relaxed);
}

std::vector<uint16_t> surface_collection::gen_surface_element_buffer() noexcept {
  const uint32_t vert_count = static_cast<uint32_t>(vertex_details_ * vertex_details_);
  std::vector<uint16_t> buffer(vert_count * 6);
  auto it = buffer.begin();
  for (size_t i = 0; i < vertex_details_; ++i) {
    for (size_t j = 0; j < vertex_details_; ++j) {
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

std::vector<uint16_t> surface_collection::gen_bounding_box_element_buffer() noexcept {
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

uint16_t surface_collection::to_vertex_index(size_t row, size_t col) const noexcept {

  return static_cast<uint16_t>(row * (vertex_details_ + 1) + col);
}

void surface_collection::adjust_candidates() noexcept {
  auto prev_candidates = std::move(candidate_tiles_);

  for (auto &node : prev_candidates) {
    auto [too_far, too_near] = node->is_enough_resolution(last_frame_);
    if (too_far) {
      candidate_tiles_.emplace(node->collapse());
    } else if (too_near) {
      auto children = node->expand();
      candidate_tiles_.insert(children.begin(), children.end());
    } else {
      candidate_tiles_.emplace(node);
    }
  }

  std::vector<rptr<surface_tile>> slice_tiles;
  for (auto &node : candidate_tiles_) {
    auto parent = node->collapse();
    if (candidate_tiles_.count(parent)) {
      /// the parent existence means
      /// there exists at least one collapsed brother before,
      /// ignore the current node therefore.
      slice_tiles.emplace_back(node);
    }
  }

  for (auto &node : slice_tiles) {
    candidate_tiles_.erase(node);
  }
}

void surface_collection::prepare_render() noexcept {
  frame_info next_frame;
  size_t event_count = 0;
  while (updating_queue_.try_pop(next_frame)) {
    ++event_count;
  }

  if (event_count > 0 && next_frame != last_frame_) {
    last_frame_ = std::move(next_frame);
    adjust_candidates();
  }

  if (next_frame_prepared_.load(std::memory_order_acquire)) {
    std::this_thread::yield();
  } else {
    next_frame_tiles_.clear();
    for (auto &node : candidate_tiles_) {
      if (!node->is_ready_to_render()) {
        node->gen_vertex_buffer(vertex_details_);
      }

      if (node->is_visible(last_frame_)) {
        next_frame_tiles_.emplace_back(node);
      }

    }
    next_frame_prepared_.store(true, std::memory_order_release);
  }
}

} // namespace scene

} // namespace esim
