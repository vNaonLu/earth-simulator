#ifndef __ESIM_SCENE_SURFACE_LAYER_H_
#define __ESIM_SCENE_SURFACE_LAYER_H_

#include <camera.h>
#include <memory>
#include <utils.h>

namespace esim {

namespace scene {

class surface_layer {
public:
  /**
   * @brief Draw the surface scene
   * 
   * @param cmr specifies the camera information.
   */
  void draw(const camera &cmr) noexcept;

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