#ifndef __ESIM_SCENE_SCENE_ENGINE_H_
#define __ESIM_SCENE_SCENE_ENGINE_H_

#include "scene_message.h"
#include <functional>
#include <utils.h>
#include <utils/observer.h>

namespace esim {

namespace scene {

/**
 * @brief The scene manager with responsibility to controll the whole
 * rendering space.
 *
 */
class scene_engine : public utils::observer<u_ptr<scene_message>> {
public:
  /**
   * @brief Render the whole scene endless
   * 
   * @param before_rd specifies the callback before rendering a frame.
   * @param swap_buffer specifies the callback to swap buffer.
   * @param after_rd specifies the callback after rendering a frame.
   */
  void start(std::function<void()> before_rd,
             std::function<void()> swap_buffer,
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
   * @brief Construct a new scene engine object
   * 
   */
  scene_engine() noexcept;

  /**
   * @brief Destroy the scene engine object
   * 
   */
  ~scene_engine() noexcept;

protected:
  void update(u_ptr<scene_message> &&msg) noexcept final;

private:
  class impl;
  u_ptr<impl> pimpl_;
};

} // namespace scene

} // namespace esim

#endif