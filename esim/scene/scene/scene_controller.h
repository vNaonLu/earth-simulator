#ifndef __ESIM_SCENE_SCENE_SCENE_CONTROLLER_H_
#define __ESIM_SCENE_SCENE_SCENE_CONTROLLER_H_

#include "scene_message.h"
#include "utils/camera.h"
#include "utils/observer.h"
#include "utils/utils.h"
#include <functional>

namespace esim {

namespace scene {

/**
 * @brief The scene control manager with responsibility to adjust
 * camera position from mouse or keyboard input.
 *
 */
class scene_controller : public utils::observer<scene_controller> {
public:
  /**
   * @brief Obtain the scene camera
   * 
   * @return the camera object.
   */
  const camera &camera() const noexcept;

  /**
   * @brief Stop the event thread
   * 
   */
  void stop() noexcept;

  /**
   * @brief Update the viewport size
   * 
   * @param width specifies the width of viewport .
   * @param height specifies the height of viewport.
   */
  void update_viewport(int width, int height) noexcept;

  /**
   * @brief Zooming the camera
   * 
   * @param tick specifies the zoom tick in [1, 9].
   */
  void zooming(double tick) noexcept;

  /**
   * @brief Press the keyboard
   * 
   * @param key specifies the target keycode.
   */
  void press(int key) noexcept;
  
  /**
   * @brief Release the keyboard
   * 
   * @param key specifies the target keycode.
   */
  void release(int key) noexcept;

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

protected:
  void on_receive(r_ptr<void>) noexcept;

private:
  class impl;
  u_ptr<impl> pimpl_;
  friend class utils::observer_attorney<scene_controller>;
};

} // namespace scene

} // namespace esim

#endif