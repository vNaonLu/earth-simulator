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
    /// finally cause non-stop
    state_.fetch_xor(enums::to_raw(state::working), std::memory_order_release);
  }
}

void esim_controller::opaque::push_event(protocol::event event) noexcept {
  event_queue_.push(event);
}

esim_controller::opaque::opaque(
    std::function<void(rptr<void>)> notify_callback) noexcept
    : frame_info_{}, left_mouse_pressed_{false},
      info_callback_{notify_callback},
      event_queue_{1024}, state_{0} {
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
  auto info = reinterpret_cast<rptr<scene::frame_info>>(msg);
  frame_info_.update_cursor(*info);
  if (frame_info_.expect_redraw(*info)) {
    send_event();
  } else {
    state_.fetch_and(~enums::to_raw(state::pause), std::memory_order_release);
  }
}

void esim_controller::opaque::send_event() noexcept {
  scene::frame_info fork = frame_info_;
  info_callback_(&fork);
}

void esim_controller::opaque::event_reset() noexcept {
  pressed_keys_.clear();
  left_mouse_pressed_ = false;
}

bool esim_controller::opaque::calculate_zoom() noexcept {
  using namespace glm;

  int rise = 0;

  if (pressed_keys_.count(protocol::KEY_PAGEUP)) {
    ++rise;
  }
  if (pressed_keys_.count(protocol::KEY_PAGEDOWN)) {
    --rise;
  }

  if (rise != 0) {
    auto &cmr = frame_info_.camera;
    dvec3 ground;
    auto pos = cmr.pos<double>(), dir = cmr.dir<double>(), up = cmr.up<double>();

    geo::ecef_to_geo(pos, ground); ground.z = 100000.0f;
    geo::geo_to_ecef(ground, ground);
    
    pos += (ground - pos) / 100.0 * static_cast<double>(rise);
    cmr.set_camera(pos, dir, up);

    return true;
  }

  return false;
}

bool esim_controller::opaque::calculate_rotation() noexcept {
    using namespace glm;
    constexpr static double kMaxROTHeight = 20'000'000.f;

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

      dvec3  LLA = pos;
      double alt = min(geo::ecef_to_geo(pos, LLA).z, kMaxROTHeight);
      double ROT = 1.0 * alt / kMaxROTHeight;

      dmat4x4 rotate_mat = rotate(dmat4x4{1.0f}, radians(ROT), normalize(rotate_axis));

      pos = rotate_mat * dvec4{pos, 1.0f};
      dir = rotate_mat * dvec4{dir, 0.0f};
      up  = rotate_mat * dvec4{up, 0.0f};

      cmr.set_camera(pos, dir, up);

      return true;
    }

    return false;
}

bool esim_controller::opaque::calculate_motion() noexcept {
  frame_info_.is_moving =  calculate_zoom() | calculate_rotation();
  
  return frame_info_.is_moving;
}

bool esim_controller::opaque::event_key_press(protocol::keycode_type key) noexcept {
  pressed_keys_.insert(key);

  switch (key) {
  case protocol::KEY_ONE:
    frame_info_.debug_show_scene = !frame_info_.debug_show_scene;
    return true;

  case protocol::KEY_TWO:
    frame_info_.debug_show_light = !frame_info_.debug_show_light;
    return true;

  case protocol::KEY_THREE:
    frame_info_.debug_show_ndc = !frame_info_.debug_show_ndc;
    return true;
    
  case protocol::KEY_B:
    frame_info_.debug_show_box = !frame_info_.debug_show_box;
    return true;
  
  default:

    return false;
  }
}

bool esim_controller::opaque::event_key_release(protocol::keycode_type key) noexcept {
  pressed_keys_.erase(key);

  return false;
}

bool esim_controller::opaque::event_mouse_move(double x, double y) noexcept {
  frame_info_.cursor_pos_.x = x;
  frame_info_.cursor_pos_.y = y;
    
  return true;
}

bool esim_controller::opaque::event_mouse_left_press() noexcept {
  left_mouse_pressed_ = true;
  return true;
}

bool esim_controller::opaque::event_mouse_left_release() noexcept {
  left_mouse_pressed_ = false;

  return false;
}

bool esim_controller::opaque::event_perform(const protocol::event &event) noexcept {
  auto &cmr = frame_info_.camera;
  bool resend_event = false;
  switch (event.type) {
  case protocol::EVENT_ZOOM:
    event_reset();
    break;
  case protocol::EVENT_VIEWPORT:
    event_reset();
    cmr.set_viewport(event.x, event.y);
    break;
  case protocol::EVENT_KEYPRESS:
    resend_event |= event_key_press(event.key);
    break;
  case protocol::EVENT_KEYRELEASE:
    resend_event |= event_key_release(event.key);
    break;
  case protocol::EVENT_MOUSEMOVE:
    resend_event |= event_mouse_move(event.cursor_x, event.cursor_y);
    break;
  case protocol::EVENT_MOUSELEFTPRESS:
    resend_event |= event_mouse_left_press();
    break;
  case protocol::EVENT_MOUSELEFTRELEASE:
    resend_event |= event_mouse_left_release();
    break;
  }

  return resend_event;
}

void esim_controller::opaque::event_handler() noexcept {
  static protocol::event event_msg;
  while(is_working()) {
    bool resend_event = false;

    if (!is_pause()) {
      while (event_queue_.try_pop(event_msg)) {
        resend_event |= event_perform(event_msg);
      }

      resend_event |= calculate_motion();

      if (resend_event) {
        send_event();
        state_.fetch_or(enums::to_raw(state::pause), std::memory_order_release);
      }
    }
  }
}

} // namespace esim
