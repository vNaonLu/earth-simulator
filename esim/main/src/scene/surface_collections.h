#ifndef __ESIM_MAIN_SOURCE_SCENE_SURFACE_COLLECTION_H_
#define __ESIM_MAIN_SOURCE_SCENE_SURFACE_COLLECTION_H_

#include "core/utils.h"
#include "glapi/buffer.h"
#include "programs/surface_program.h"
#include "scene_entity.h"
#include "surface_tile.h"
#include <unordered_map>

namespace esim {

namespace scene {

class surface_collection final : public scene_entiy {
public:
  void render(const scene::frame_info &info) noexcept final;

  void render_bounding_box(const scene::frame_info &info) noexcept;

  surface_collection(size_t vertex_details) noexcept;

  ~surface_collection() noexcept;

private:
  std::vector<uint16_t> gen_surface_element_buffer() noexcept;

  std::vector<uint16_t> gen_bounding_box_element_buffer() noexcept;

  uint16_t to_vertex_index(size_t row, size_t col) const noexcept;

private:
  size_t                          vertex_details_;
  gl::buffer<uint16_t>            ebo_;
  std::vector<uptr<surface_tile>> tiles_;
};

} // namespace scene

} // namespace esim

#endif