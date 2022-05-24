#include "esim/esim_controller.h"
#include "esim_controller_opaque.h"

namespace esim {

namespace details {

static std::function<void()> before_render_callback = []() {};

static std::function<void()> after_render_callback = []() {};

} // namespace details

void esim_controller::start() noexcept {
  assert(nullptr != engine_);
  assert(nullptr != opaque_);

  if (! opaque_->start()) {
    assert(false);
    return;
  }

  engine_->start([&]() { details::before_render_callback(); }, [&]() { details::after_render_callback(); });
}

void esim_controller::stop() noexcept {
  assert(nullptr != engine_);
  assert(nullptr != opaque_);
  opaque_->stop();
  engine_->stop();
}

void esim_controller::render() noexcept {
  assert(nullptr != engine_);
  engine_->render();
}

void esim_controller::update_viewport(int width, int height) noexcept {
  assert(nullptr != opaque_);
  protocol::event ev;
  ev.type = protocol::EVENT_VIEWPORT;
  ev.x = width; ev.y = height;
  opaque_->push_event(ev);
}

void esim_controller::zoom(double tick) noexcept {
  assert(nullptr != opaque_);
  protocol::event ev;
  ev.type = protocol::EVENT_ZOOM;
  ev.value = tick;
  opaque_->push_event(ev);
}

void esim_controller::key_press(protocol::keycode_type key) noexcept {
  assert(nullptr != opaque_);
  protocol::event ev;
  ev.type = protocol::EVENT_KEYPRESS;
  ev.key = key;
  opaque_->push_event(ev);
}

void esim_controller::key_release(protocol::keycode_type key) noexcept {
  assert(nullptr != opaque_);
  protocol::event ev;
  ev.type = protocol::EVENT_KEYRELEASE;
  ev.key = key;
  opaque_->push_event(ev);
}

void esim_controller::mouse_move(double x, double y) noexcept {
  assert(nullptr != opaque_);
  protocol::event ev;
  ev.type = protocol::EVENT_MOUSEMOVE;
  ev.cursor_x = x;
  ev.cursor_y = y;
  opaque_->push_event(ev);
}

void esim_controller::left_mouse_press() noexcept {
  assert(nullptr != opaque_);
  protocol::event ev;
  ev.type = protocol::EVENT_MOUSELEFTPRESS;
  opaque_->push_event(ev);
}

void esim_controller::left_mouse_release() noexcept {
  assert(nullptr != opaque_);
  protocol::event ev;
  ev.type = protocol::EVENT_MOUSELEFTRELEASE;
  opaque_->push_event(ev);
}


void esim_controller::bind_before_render_process(std::function<void()> callback) noexcept {
  details::before_render_callback = callback;
}

void esim_controller::bind_after_render_process(std::function<void()> callback) noexcept {
  details::after_render_callback = callback;
}

esim_controller::esim_controller() noexcept
    : opaque_{make_uptr<opaque>([&](rptr<void> msg) { notify(msg); })},
      engine_{make_uptr<esim_engine>()} {
  subscribe(engine_.get());
  engine_->subscribe(this);
}

esim_controller::~esim_controller() noexcept {
  assert(nullptr != opaque_);
  opaque_->stop();
}

void esim_controller::update(rptr<void> msg) noexcept {
  assert(nullptr != opaque_);
  opaque_->receive_last_frame(msg);
}

}