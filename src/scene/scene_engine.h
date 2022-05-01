#ifndef __ESIM_SCENE_SCENE_ENGINE_H_
#define __ESIM_SCENE_SCENE_ENGINE_H_

#include <functional>
#include <utils.h>

namespace esim {

namespace scene {

/**
 * @brief The scene manager with responsibility to controll the whole
 * rendering space.
 *
 */
class scene_engine final {
public:
  /**
   * @brief Update the viewport size
   * 
   * @param width specifies the width of viewport.
   * @param height specifies the height of viewport.
   */
  void update_viewport(uint32_t width, uint32_t height) noexcept;

  /**
   * @brief Render the whole scene endless
   * 
   * @param before_rd specifies the callback before rendering a frame.
   * @param after_rd specifies the callback after rendering a frame.
   */
  void start(std::function<void()> before_rd,
             std::function<void()> after_rd) noexcept;

  /**
   * @brief Puase rendering
   * 
   */
  void pause() noexcept;

  /**
   * @brief Resume rendering
   * 
   */
  void resume() noexcept;
  
  /**
   * @brief Stop rendering and return the loop
   * 
   */
  void stop() noexcept;

  /**
   * @brief Zoom in the scene
   * 
   * @param tick specifies the zoom level
   */
  void zoom_in(double tick) noexcept;
  
  /**
   * @brief Zoom out the scene
   * 
   * @param tick specifies the zoom level
   */
  void zoom_out(double tick) noexcept;

  /**
   * @brief Construct a new scene engine object
   * 
   */
  scene_engine() noexcept;

  /**
   * @brief Destroy the scene engine object
   * 
   */
  ~scene_engine() noexcept;

private:
  class impl;
  u_ptr<impl> pimpl_;
};

} // namespace scene

} // namespace esim

#endif