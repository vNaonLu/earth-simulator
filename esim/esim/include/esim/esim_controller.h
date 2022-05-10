#ifndef __ESIM_ESIM_ESIM_ESIM_CONTROLLER_H_
#define __ESIM_ESIM_ESIM_ESIM_CONTROLLER_H_

#include "core/observer.h"
#include "core/subject.h"
#include "core/utils.h"
#include "protocol.h"
#include "scene/information.h"

namespace esim {

/**
 * @brief Earth controller managering the mouse and keyboard input
 * 
 */
class esim_controller final
    : public core::subject,
      public core::observer {
public:

  /**
   * @brief Start the event handling.
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