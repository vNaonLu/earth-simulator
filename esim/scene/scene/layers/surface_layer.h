#ifndef __ESIM_SCENE_SCENE_LAYERS_SURFACE_LAYER_H_
#define __ESIM_SCENE_SCENE_LAYERS_SURFACE_LAYER_H_

#include "scene/render/rendering_infos.h"
#include "utils/camera.h"
#include "utils/utils.h"

namespace esim {

namespace scene {

class surface_layer {
public:
  /**
   * @brief Draw the surface scene
   * 
   * @param cmr specifies the rendering information.
   */
  void draw(const rendering_infos &info) noexcept;

  /**
   * @brief Draw the bounding box of surface layer
   * 
   * @param cmr specifies the rendering information.
   */
  void draw_bounding_box(const rendering_infos &info) noexcept;

  /**
   * @brief Construct a new surface layer object
   * 
   * @param node_details specifies the details of surface_node.
   */
  surface_layer(size_t node_details = 33) noexcept;

  /**
   * @brief Destroy the surface layer object
   * 
   */
  ~surface_layer() noexcept;

private:
  class impl;
  u_ptr<impl> pimpl;
};

} // namespace scene

} // namespace esim

#endif