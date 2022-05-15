#include "esim_controller_opaque.h"

namespace esim {

bool esim_controller::opaque::is_working() const noexcept {
  using namespace enums;
  return status() & state::working;
}

bool esim_controller::opaque::is_pause() const noexcept {
  using namespace enums;
  return status() & state::pause;
}

bool esim_controller::opaque::is_terminated() const noexcept {
  using namespace enums;
  return status() & state::terminated;
}

bool esim_controller::opaque::start() noexcept {
  if (is_working() || is_terminated()) {

    return false;
  }
  state_.fetch_or(enums::to_raw(state::working), std::memory_order_release);
  std::thread([&]() { event_handler(); }).detach();

  return true;
}

void esim_controller::opaque::stop() noexcept {
  if (is_working()) {
    /// multiple thread may repeatly xor
    /// finally non-stop
    state_.fetch_xor(enums::to_raw(state::working), std::memory_order_release);
  }
}

void esim_controller::opaque::push_event(protocol::event event) noexcept {
  event_queue_.push(event);
}

esim_controller::opaque::opaque(
    std::function<void(rptr<void>)> notify_callback) noexcept
    : frame_info_{}, zoom_tick_{0.0f},
      info_callback_{notify_callback},
      event_queue_{512}, state_{0} {
  assert(nullptr != info_callback_);
}

esim_controller::opaque::~opaque() noexcept {
  stop();
}

enums::raw<esim_controller::opaque::state>
esim_controller::opaque::status(std::memory_order mo) const noexcept {

  return state_.load(mo);
}

void esim_controller::opaque::receive_last_frame(rptr<void> msg) noexcept {
  state_.fetch_xor(enums::to_raw(state::pause), std::memory_order_release);
  auto info = reinterpret_cast<rptr<scene::frame_info>>(msg);
  if (frame_info_.camera != info->camera ||
      frame_info_.sun != info->sun) {
    redraw_event();
  }
}

void esim_controller::opaque::redraw_event() noexcept {
  scene::frame_info fork = frame_info_;
  info_callback_(&fork);
  state_.fetch_xor(enums::to_raw(state::pause), std::memory_order_release);
}

void esim_controller::opaque::event_reset() noexcept {
  zoom_tick_ = 0.0f;
  pressed_keys_.clear();
}

void esim_controller::opaque::calculate_zoom() noexcept {
  using namespace glm;
  if (zoom_tick_ != 0.0f) {
    auto &cmr = frame_info_.camera;
    dvec3 ground;
    auto pos = cmr.pos<double>(), dir = cmr.dir<double>(), up = cmr.up<double>();

    geo::ecef_to_geo(pos, ground); ground.z = 0.0f;
    geo::geo_to_ecef(ground, ground);
    
    pos += (ground - pos) / 10.0 * zoom_tick_;
    cmr.set_camera(pos, dir, up);
    zoom_tick_ = 0.0f;
    redraw_event();
  }
}

void esim_controller::opaque::calculate_rotation() noexcept {
    using namespace glm;
    ivec2 move_angle_offset(0);

    if (pressed_keys_.count(protocol::KEY_LEFT)) {
      --move_angle_offset.x;
    }
    if (pressed_keys_.count(protocol::KEY_UP)) {
      ++move_angle_offset.y;
    }
    if (pressed_keys_.count(protocol::KEY_RIGHT)) {
      ++move_angle_offset.x;
    }
    if (pressed_keys_.count(protocol::KEY_DOWN)) {
      --move_angle_offset.y;
    }

    if (move_angle_offset.x != 0 || move_angle_offset.y != 0) {
      auto &cmr = frame_info_.camera;
      auto pos = cmr.pos<double>(), dir = cmr.dir<double>(), up = cmr.up<double>();

      dvec3 hori_rotate_axis = up * double(move_angle_offset.x);
      dvec3 vert_rotate_axis = cross(up, dir) * double(move_angle_offset.y);
      dvec3 rotate_axis = hori_rotate_axis + vert_rotate_axis;
      dmat4x4 rotate_mat = rotate(dmat4x4{1.0f}, static_cast<double>(radians(0.3f * 3)), normalize(rotate_axis));

      pos = rotate_mat * dvec4{pos, 1.0f};
      dir = rotate_mat * dvec4{dir, 0.0f};
      up = rotate_mat * dvec4{up, 0.0f};

      cmr.set_camera(pos, dir, up);
      redraw_event();
    }
}

void esim_controller::opaque::calculate_motion() noexcept {
  calculate_zoom();
  calculate_rotation();
}

void esim_controller::opaque::event_key_press(protocol::keycode_type key) noexcept {
  pressed_keys_.insert(key);

  switch (key) {
  case protocol::KEY_ONE:
    frame_info_.debug_show_scene = !frame_info_.debug_show_scene;
    redraw_event();
    break;

  case protocol::KEY_TWO:
    frame_info_.debug_show_light = !frame_info_.debug_show_light;
    redraw_event();
    break;
  
  default:
    break;
  }
}

void esim_controller::opaque::event_key_release(protocol::keycode_type key) noexcept {
  pressed_keys_.erase(key);
}

void esim_controller::opaque::event_perform(const protocol::event &event) noexcept {
  auto &cmr = frame_info_.camera;
  switch (event.type) {
  case protocol::EVENT_ZOOM:
    event_reset();
    zoom_tick_ = event.value;
    break;
  case protocol::EVENT_VIEWPORT:
    event_reset();
    cmr.set_viewport(event.x, event.y);
    break;
  case protocol::EVENT_KEYPRESS:
    event_key_press(event.key);
    break;
  case protocol::EVENT_KEYRELEASE:
    event_key_release(event.key);
    break;
  }
}

void esim_controller::opaque::event_handler() noexcept {
  static protocol::event event_msg;
  while(is_working()) {
    if (!is_pause()) {
      if (event_queue_.try_pop(event_msg)) {
        event_perform(event_msg);
      }

      calculate_motion();
    }
  }
}

} // namespace esim
