#ifndef __ESIM_MAIN_SOURCE_SCENE_SURFACE_COLLECTION_H_
#define __ESIM_MAIN_SOURCE_SCENE_SURFACE_COLLECTION_H_

#include "core/fifo.h"
#include "core/utils.h"
#include "details/basemap_storage.h"
#include "glapi/buffer.h"
#include "programs/surface_program.h"
#include "scene_entity.h"
#include "surface_tile.h"
#include <atomic>
#include <thread>
#include <unordered_map>
#include <unordered_set>

namespace esim {

namespace scene {

class surface_collection final : public scene_entity {
public:
  void render(const scene::frame_info &info) noexcept final;

  void render_bounding_box(const scene::frame_info &info) noexcept;

  surface_collection(size_t vertex_details) noexcept;

  ~surface_collection() noexcept;

private:
  std::vector<uint16_t> gen_surface_element_buffer() noexcept;

  std::vector<uint16_t> gen_bounding_box_element_buffer() noexcept;

  uint16_t to_vertex_index(size_t row, size_t col) const noexcept;

  /// layer-threading
  void adjust_candidates() noexcept;

  void prepare_render() noexcept;

private:
  size_t                                 vertex_details_;
  gl::buffer<uint16_t>                   ebo_;
  std::atomic<bool>                      next_frame_prepared_, is_working_;
  uptr<surface_tile>                     surface_root_;
  std::vector<rptr<surface_tile>>        render_tiles_, next_frame_tiles_;
  std::unordered_set<rptr<surface_tile>> candidate_tiles_;
  basemap_storage                        basemaps_;
  
  core::fifo<frame_info> updating_queue_;
  frame_info             last_frame_;
};

} // namespace scene

} // namespace esim

#endif