#ifndef __ESIM_SCENE_SCENE_SCENE_RENDERER_H_
#define __ESIM_SCENE_SCENE_SCENE_RENDERER_H_

#include "utils/camera.h"
#include "utils/utils.h"

namespace esim {

namespace scene {

/**
 * @brief The rendering engine of scene with resposibility to
 * control the OpenGl api and the render rule of layers
 *
 */
class scene_renderer {
public:
  /**
   * @brief Render a frame of the scene
   * 
   * @param cmr specifies the camera information.
   */
  void render(const camera &cmr) noexcept;

  /**
   * @brief Construct a new scene renderer object
   * 
   */
  scene_renderer() noexcept;

  /**
   * @brief Destroy the scene renderer object
   * 
   */
  ~scene_renderer() noexcept;

private:
  class impl;
  u_ptr<impl> pimpl_;
};

} // namespace scene

} // namespace esim

#endif