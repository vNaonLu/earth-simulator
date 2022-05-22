#include "scene/surface_collections.h"
#include "programs/bounding_box_program.h"

namespace esim {

namespace scene {

void surface_collection::render(const scene::frame_info &info) noexcept {
  auto program = program::surface_program::get();
  program->use();
  program->update_common_uniform(info);

  updating_queue_.try_push(info);
  if (next_frame_prepared_.load(std::memory_order_acquire)) {
    render_tiles_.swap(next_frame_tiles_);
    next_frame_prepared_.store(false, std::memory_order_release);
  }

  for (auto &node : render_tiles_) {
    auto [basemap, texinfo] = basemaps_.get(node->details(), !info.is_moving);
    program->update_basemap_uniform(basemap, texinfo);
    ebo_.bind(0); node->render(info, ebo_.size(0));
    ebo_.bind(1); node->render(info, ebo_.size(1));
  }
}

void surface_collection::render_bounding_box([[maybe_unused]] const scene::frame_info &info) noexcept {
  using namespace glm;
  auto program = program::bounding_box_program::get();
  ebo_.bind(2);
  program->use();
  program->update_common_uniform(info);
  program->update_line_color_uniform(vec4{0.0, 1.0, 0.0, 0.8});
  for (auto &node : render_tiles_) {
    node->render_bounding_box(info, ebo_.size(2));
  }
}

surface_collection::surface_collection(size_t vertex_details) noexcept
    : vertex_details_{vertex_details}, ebo_{GL_ELEMENT_ARRAY_BUFFER, 3},
      next_frame_prepared_{false}, is_working_{false},
      surface_root_{make_uptr<surface_tile>(geo::maptile{0, 0, 0})},
      basemaps_{"server.arcgisonline.com", "/arcgis/rest/services/World_Imagery/MapServer/tile/{z}/{x}/{y}", 16}, 
      surface_vertices_engine_{make_uptr<surface_vertex_engine>(30)}, updating_queue_{256} {
  ebo_.bind_buffer(surface_vertices_engine_->export_center_element_buffer(), GL_STATIC_DRAW, 0);
  ebo_.bind_buffer(surface_vertices_engine_->export_skirt_element_buffer(), GL_STATIC_DRAW, 1);
  ebo_.bind_buffer(surface_vertices_engine_->export_obb_element_buffer(), GL_STATIC_DRAW, 2);
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

    if (next_frame_prepared_.load(std::memory_order_acquire)) {
      std::this_thread::yield();
    } else {
      next_frame_tiles_.clear();
      for (auto &node : candidate_tiles_) {
        if (!node->is_ready_to_render()) {
          node->gen_vertex_buffer(surface_vertices_engine_->gen_surface_vertices(node->details()));
        }

        if (node->is_visible(last_frame_)) {
          next_frame_tiles_.emplace_back(node);
        }

      }
      next_frame_prepared_.store(true, std::memory_order_release);
    }
  }
}

} // namespace scene

} // namespace esim
