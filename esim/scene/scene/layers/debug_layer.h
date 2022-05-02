#ifndef __ESIM_SCENE_LAYER_DEBUG_LAYER_H_
#define __ESIM_SCENE_LAYER_DEBUG_LAYER_H_

#include "utils/camera.h"
#include "utils/utils.h"

namespace esim {

namespace scene {

class debug_layer {
public:
  /**
   * @brief Draw the surface scene
   * 
   * @param cmr specifies the camera information.
   */
  void draw(const camera &cmr) noexcept;

  /**
   * @brief Construct a new surface layer object
   */
  debug_layer() noexcept;

  /**
   * @brief Destroy the surface layer object
   * 
   */
  ~debug_layer() noexcept;

private:
  class impl;
  u_ptr<impl> pimpl_;
};

} // namespace scene

} // namespace esim

#endif