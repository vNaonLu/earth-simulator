#include "esim/esim_controller.h"
#include "esim_controller_opaque.h"

namespace esim {

inline bool esim_controller::start() noexcept {
  assert(nullptr != opaque_);
  return opaque_->start();
}

inline void esim_controller::stop() noexcept {
  assert(nullptr != opaque_);
  opaque_->stop();
}

inline void esim_controller::update_viewport(int width, int height) noexcept {
  assert(nullptr != opaque_);
  protocol::event ev;
  ev.type = protocol::EVENT_ZOOM;
  ev.x = width; ev.y = height;
  opaque_->push_event(ev);
}

inline void esim_controller::zoom(double tick) noexcept {
  assert(nullptr != opaque_);
  protocol::event ev;
  ev.type = protocol::EVENT_ZOOM;
  ev.value = tick;
  opaque_->push_event(ev);
}

inline void esim_controller::key_press(protocol::keycode_type key) noexcept {
  assert(nullptr != opaque_);
  protocol::event ev;
  ev.type = protocol::EVENT_KEYPRESS;
  ev.key = key;
  opaque_->push_event(ev);
}

inline void esim_controller::key_press(protocol::keycode_type key) noexcept {
  assert(nullptr != opaque_);
  protocol::event ev;
  ev.type = protocol::EVENT_KEYRELEASE;
  ev.key = key;
  opaque_->push_event(ev);
}

inline esim_controller::esim_controller() noexcept
    : opaque_{make_uptr<opaque>([&](rptr<void> msg) { notify(msg); })} {}

inline esim_controller::~esim_controller() noexcept {
  assert(nullptr != opaque_);
  opaque_->stop();
}

inline void esim_controller::update(rptr<void> msg) noexcept {
  assert(nullptr != opaque_);
  opaque_->receive_last_frame(msg);
}

}