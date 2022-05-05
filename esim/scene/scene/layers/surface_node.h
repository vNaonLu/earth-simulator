#ifndef __ESIM_SCENE_SCENE_LAYERS_SURFACE_NODE_H_
#define __ESIM_SCENE_SCENE_LAYERS_SURFACE_NODE_H_

#include "scene/render/rendering_infos.h"
#include "utils/camera.h"
#include "utils/utils.h"
#include <cstddef>

namespace esim {

namespace scene {

/**
 * @brief maptile structure
 * 
 */
struct maptile {
  uint32_t zoom,
           xtile, ytile;
};

/**
 * @brief Surface of earth drawing node
 * 
 */
class surface_node {
public:
  /**
   * @brief Draw surface node scene
   * 
   * @param cmr specifies the rendering information.
   * @param indices_count specifies the indices count.
   */
  void draw(const rendering_infos &info, size_t indices_count) noexcept;

  /**
   * @brief Draw the surface grid
   * 
   * @param cmr specifies the rendering information.
   * @param indices_count specifies the indices count.
   */
  void draw_grid(const rendering_infos &info, size_t indices_count) noexcept;
  
  /**
   * @brief Draw the bounding box
   * 
   * @param cmr specifies the rendering information.
   * @param indices_count specifies the indices count.
   */
  void draw_bounding_box(const rendering_infos &info, size_t indices_count) noexcept;

  /**
   * @brief Generate vertex buffer
   * 
   * @param vertex_details specifies the details value.
   */
  void gen_vertex(size_t vertex_details) noexcept;

  /**
   * @brief Construct a new surface node object
   * 
   * @param tile specifies the map tile information.
   */
  surface_node(maptile tile) noexcept;

  /**
   * @brief Destroy the surface node object
   * 
   */
  ~surface_node() noexcept;

private:
  class impl;
  u_ptr<impl> pimpl;
};

} // namespace scene

} // namespace esim

#endif