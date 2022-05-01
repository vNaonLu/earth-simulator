#ifndef __ESIM_SCENE_SCENE_CONTROLLER_H_
#define __ESIM_SCENE_SCENE_CONTROLLER_H_

#include <camera.h>
#include <utils.h>

namespace esim {

namespace scene {

/**
 * @brief The scene control manager with responsibility to adjust
 * camera position from mouse or keyboard input.
 *
 */
class scene_controller {
public:
  /**
   * @brief Obtain the scene camera
   * 
   * @return the camera object.
   */
  const camera &camera() const noexcept;

  /**
   * @brief Update the viewport size
   * 
   * @param width specifies the width of viewport .
   * @param height specifies the height of viewport.
   */
  void update_viewport(uint32_t width, uint32_t height) noexcept;

  /**
   * @brief Zoom in the camera
   * 
   * @param tick specifies the zoom tick in [1, 9].
   */
  void zoom_in(double tick) noexcept;
  
  /**
   * @brief Zoom out the camera
   * 
   * @param tick specifies the zoom tick in [1, 9].
   */
  void zoom_out(double tick) noexcept;

  /**
   * @brief Construct a new scene controller object
   * 
   */
  scene_controller() noexcept;

  /**
   * @brief Destroy the scene controller object
   * 
   */
  ~scene_controller() noexcept;

private:
  class impl;
  u_ptr<impl> pimpl_;
};

} // namespace scene

} // namespace esim

#endif