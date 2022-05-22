#ifndef __ESIM_MAIN_ESIM_ESIM_CONTROLLER_H_
#define __ESIM_MAIN_ESIM_ESIM_CONTROLLER_H_

#include "core/observer.h"
#include "core/publisher.h"
#include "core/utils.h"
#include "details/information.h"
#include "details/protocol.h"
#include "esim_engine.h"
#include <cassert>
#include <functional>

namespace esim {

/**
 * @brief Earth controller managering the mouse and keyboard input
 * 
 */
class esim_controller final : public core::publisher,
                              public core::observer {
public:

  /**
   * @brief Start a thread to handle the events and render the frame.
   * 
   */
  void start() noexcept;

  /**
   * @brief Force to render a frame.
   * 
   */
  void render() noexcept;

  /**
   * @brief Stop the event handling and rendering.
   * 
   */
  void stop() noexcept;

  /**
   * @brief Spawn a updating viewport event.
   * 
   * @param width specifies the width.
   * @param height specifies the height.
   */
  void update_viewport(int width, int height) noexcept;

  /**
   * @brief Spawn a zooming event.
   * 
   * @param tick specifies the zoom value.
   */
  void zoom(double tick) noexcept;

  /**
   * @brief Spawn a key-pressing event.
   * 
   * @param key specifies the keycode.
   */
  void key_press(protocol::keycode_type key) noexcept;
  
  /**
   * @brief Spawn a key-releasing event.
   * 
   * @param key specifies the keycode.
   */
  void key_release(protocol::keycode_type key) noexcept;

  /**
   * @brief Spawn a mouse-moving event.
   * 
   * @param x specifies the horizontal coordinate of cursor.
   * @param y specifies the vertical coordinate of cursor.
   */
  void mouse_move(double x, double y) noexcept;

  /**
   * @brief Bind the callback before rendering a frame.
   * 
   * @param callback specifies the callback before rendering.
   */
  void bind_before_render_process(std::function<void()> callback) noexcept;

  /**
   * @brief Bind the callback after rendered a frame.
   * 
   * @param callback specifies the callback after rendered.
   */
  void bind_after_render_process(std::function<void()> callback) noexcept;

  /**
   * @brief Construct a new esim controller object.
   * 
   */
  esim_controller() noexcept;

  /**
   * @brief Destroy the esim controller object.
   * 
   */
  ~esim_controller() noexcept;

protected:
  void update(rptr<void> msg) noexcept final;

private:
  class opaque;
  uptr<opaque>      opaque_;
  uptr<esim_engine> engine_;
};

} // namespace esim

#endif