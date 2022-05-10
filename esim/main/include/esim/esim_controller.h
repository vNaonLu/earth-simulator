#ifndef __ESIM_MAIN_ESIM_ESIM_CONTROLLER_H_
#define __ESIM_MAIN_ESIM_ESIM_CONTROLLER_H_

#include "core/observer.h"
#include "core/publisher.h"
#include "core/utils.h"
#include "details/information.h"
#include "details/protocol.h"
#include <cassert>

namespace esim {

/**
 * @brief Earth controller managering the mouse and keyboard input
 * 
 */
class esim_controller final : public core::publisher,
                              public core::observer {
public:

  /**
   * @brief Start a thread to handle the events.
   * 
   */
  bool start() noexcept;

  /**
   * @brief Stop the event handling.
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
  uptr<opaque> opaque_;
};

} // namespace esim

#endif