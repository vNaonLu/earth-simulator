#include "scene_controller.h"
#include "transform/wgs84.h"
#include "utils/fifo.h"
#include "utils/keys.h"
#include <atomic>
#include <functional>
#include <glfw/glfw3.h>
#include <thread>
#include <unordered_set>

namespace esim {

namespace scene {

namespace details {

enum class motion_type {
  none,
  quit,
  scroll,
  viewport,
  key_press,
  key_release,
};

struct motion_event {
  motion_type type;
  union {
    double  value;

    struct {
      int x, y;
    };

    int keycode;
  };
};

} // namespace details

class scene_controller::impl {
public:
  enum class state {
    working,
    stop,
    terminated
  };

  inline const class camera &camera() const noexcept {

    return camera_;
  }

  inline void receive_response() noexcept {
    triggered_.store(false, std::memory_order_release);
  }

  inline void trigger_event() noexcept {
    auto msg = make_ptr_u<scene_message>();
    msg->camera = camera_;
    update_callback_(msg.get());
    triggered_.store(true, std::memory_order_release);
  }

  inline void moving() noexcept {
    using namespace glm;
    ivec2 move_angle_offset(0);

    if (key_pressed_.count(utils::left_key)) {
      --move_angle_offset.x;
    }
    if (key_pressed_.count(utils::up_key)) {
      ++move_angle_offset.y;
    }
    if (key_pressed_.count(utils::right_key)) {
      ++move_angle_offset.x;
    }
    if (key_pressed_.count(utils::down_key)) {
      --move_angle_offset.y;
    }

    if (move_angle_offset.x != 0 || move_angle_offset.y != 0) {
      auto [pos, dir, up] = camera_.information();

      dvec3 hori_rotate_axis = up * double(move_angle_offset.x);
      dvec3 vert_rotate_axis = cross(up, dir) * double(move_angle_offset.y);
      dvec3 rotate_axis = hori_rotate_axis + vert_rotate_axis;
      dmat4x4 rotate_mat = rotate(dmat4x4{1.0f}, static_cast<double>(radians(0.3f)), normalize(rotate_axis));

      pos = rotate_mat * dvec4{pos, 1.0f};
      dir = rotate_mat * dvec4{dir, 0.0f};
      up = rotate_mat * dvec4{up, 0.0f};

      camera_.set_camera(pos, dir, up);
      trigger_event();
    }

  }

  inline void motion_viewport(int width, int height) noexcept {
    auto &vp = camera_.viewport();
    if (vp.x != width || vp.y != height) {
      camera_.set_viewport(width, height);
      trigger_event();
    }
  }

  inline void motion_zoom(double offset) noexcept {
    if (offset != 0) {
      auto [pos, dir, up] = camera_.information();
      glm::dvec3 ground;
      trans::wgs84ecef_to_geo(pos, ground);
      ground.z = 0;
      trans::wgs84geo_to_ecef(ground, ground);

      pos += (ground - pos) / 10.0 * offset;
      camera_.set_camera(pos, dir, up);
      trigger_event();
    }
  }

  inline void press_key(int keycode) noexcept {
    key_pressed_.insert(keycode);
  }

  inline void release_key(int keycode) noexcept {
    key_pressed_.erase(keycode);
  }

  inline bool perform_motion(const details::motion_event &msg) noexcept {
    using namespace details;
    switch (msg.type) {
    case motion_type::quit:
      return false;
    case motion_type::scroll:
      reset();
      motion_zoom(msg.value);
      break;
    case motion_type::viewport:
      reset();
      motion_viewport(msg.x, msg.y);
      break;
    case motion_type::key_press:
      press_key(msg.keycode);
      break;
    case motion_type::key_release:
      release_key(msg.keycode);
      break;
    }

    return true;
  }

  inline void event_main() noexcept {
    while(state::working == service_state_.load(std::memory_order_acquire)) {
      static details::motion_event event;
      if (false == triggered_.load(std::memory_order_acquire)) {
        if (event_queue_.try_pop(event)) {
          if (!perform_motion(event)) {

            break;
          }
        }

        moving();
      }
    }
    service_state_.store(state::terminated, std::memory_order_release);
  }

  inline void push_event(details::motion_event &&msg) noexcept {
    event_queue_.push(std::move(msg));
    triggered_.store(true, std::memory_order_release);
  }

  inline void reset() noexcept {
    key_pressed_.clear();
  }

  inline void stop() noexcept {
    service_state_.store(state::stop, std::memory_order_release);
  }
  
  inline bool is_terminated() noexcept {

    return state::terminated == service_state_.load(std::memory_order_acquire);
  }

  impl(std::function<void(r_ptr<scene_message>)> cb) noexcept
      : service_state_{state::working},
        event_queue_{512},
        camera_{},
        update_callback_{cb},
        triggered_{false} {
    using namespace glm;
    auto event_pool = std::thread([&](){
      event_main();
    });
    event_pool.detach();
  }

private:
  std::atomic<state> service_state_;
  utils::fifo<details::motion_event> event_queue_;
  class camera camera_;
  std::function<void(r_ptr<scene_message>)> update_callback_;
  std::unordered_set<int> key_pressed_;
  std::atomic<bool> triggered_;
};

const class camera &scene_controller::camera() const noexcept {
  assert(nullptr != pimpl_);
  return pimpl_->camera();
}

void scene_controller::stop() noexcept {
  assert(nullptr != pimpl_);
  details::motion_event msg;
  msg.type = details::motion_type::quit;
  pimpl_->push_event(std::move(msg));
}

void scene_controller::update_viewport(int width, int height) noexcept{
  assert(nullptr != pimpl_);
  details::motion_event msg;
  msg.type = details::motion_type::viewport;
  msg.x = width; msg.y = height;
  pimpl_->push_event(std::move(msg));
}

void scene_controller::zooming(double tick) noexcept {
  assert(nullptr != pimpl_);
  details::motion_event msg;
  msg.type = details::motion_type::scroll;
  msg.value = tick;
  pimpl_->push_event(std::move(msg));
}

void scene_controller::press(int key) noexcept {
  assert(nullptr != pimpl_);
  details::motion_event msg;
  msg.type = details::motion_type::key_press;
  msg.keycode = key;
  pimpl_->push_event(std::move(msg));
}

void scene_controller::release(int key) noexcept {
  assert(nullptr != pimpl_);
  details::motion_event msg;
  msg.type = details::motion_type::key_release;
  msg.keycode = key;
  pimpl_->push_event(std::move(msg));
}

scene_controller::scene_controller() noexcept
    : pimpl_{make_ptr_u<impl>([&](r_ptr<scene_message> msg) {
                                notify(msg);
                              })} {}

scene_controller::~scene_controller() noexcept {
  assert(nullptr != pimpl_);
  pimpl_->stop();
  while(!pimpl_->is_terminated()) {
    std::this_thread::yield();
  }
}

void scene_controller::on_receive(r_ptr<void> raw) noexcept {
  assert(nullptr != pimpl_);
  auto msg = reinterpret_cast<r_ptr<scene_message>>(raw);

  pimpl_->receive_response();
  
  if (msg->camera != pimpl_->camera()) {
    /// since the draw position not equal to controller position
    /// update scene again
    auto resend_msg = make_ptr_u<scene_message>();
    resend_msg->camera = pimpl_->camera();
    notify(resend_msg.get());
  }
}

} // namespace scene

} // namespace esim
