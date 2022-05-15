#ifndef __ESIM_ESIM_SOURCE_ESIM_CONTROLLER_OPAQUE_H_
#define __ESIM_ESIM_SOURCE_ESIM_CONTROLLER_OPAQUE_H_

#include "core/fifo.h"
#include "core/transform.h"
#include "details/camera.h"
#include "esim/esim_controller.h"
#include <atomic>
#include <functional>
#include <thread>
#include <unordered_set>

namespace esim {

class esim_controller::opaque {
public:
  enum class state : uint16_t {
    initialized = 0b0,
    working     = 0b1,
    pause       = 0b10,
    terminated  = 0b100
  };

  bool is_working() const noexcept;

  bool is_pause() const noexcept;

  bool is_terminated() const noexcept;

  bool start() noexcept;

  void stop() noexcept;

  void receive_last_frame(rptr<void> msg) noexcept;
  
  void push_event(protocol::event event) noexcept;

  opaque(std::function<void(rptr<void>)> notify_callback) noexcept;

  ~opaque() noexcept;

private:
  enums::raw<state> status(std::memory_order mo = std::memory_order_acquire) const noexcept;

  void redraw_event() noexcept;

  void event_reset() noexcept;

  void calculate_zoom() noexcept;

  void calculate_rotation() noexcept;

  void calculate_motion() noexcept;

  void event_key_press(protocol::keycode_type key) noexcept;

  void event_key_release(protocol::keycode_type key) noexcept;

  void event_perform(const protocol::event &event) noexcept;

  void event_handler() noexcept;

private:
  scene::frame_info                          frame_info_;
  double                                     zoom_tick_;
  std::unordered_set<protocol::keycode_type> pressed_keys_;

  /// event handler
  std::function<void(rptr<void>)> info_callback_;
  core::fifo<protocol::event>     event_queue_;
  std::atomic<enums::raw<state>>  state_;
};

} // namespace esim

#endif