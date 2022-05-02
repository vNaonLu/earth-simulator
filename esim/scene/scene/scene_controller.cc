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
  inline const class camera &camera() const noexcept {

    return camera_;
  }

  inline void receive_response() noexcept {
    triggered_.store(false, std::memory_order_release);
  }

  inline void trigger_event() noexcept {
    update_callback_(make_ptr_u<scene_message>(camera_));
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
      dvec4 ps{camera_.ecef(), 0.0f};
      dvec4 up{camera_.up(), 0.0f}, fwd{normalize(ps)};
      dvec3 hori_rotate_axis = up * double(move_angle_offset.x);
      dvec3 vert_rotate_axis = cross(dvec3(fwd), dvec3(up)) * double(move_angle_offset.y);
      auto rotate_axis = normalize(hori_rotate_axis + vert_rotate_axis);
      auto rotate_mat = rotate(dmat4x4{1.0f},
                               static_cast<double>(radians(0.3f)),
                               rotate_axis);

      ps = rotate_mat * ps;
      up = rotate_mat * up;
      normalize(up);

      camera_.set_camera(ps, up);
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
      auto pos = camera_.ecef(),
           ground = pos;
      auto up = camera_.up();

      trans::wgs84ecef_to_geo(pos, ground);
      ground.z = 0;
      trans::wgs84geo_to_ecef(ground, ground);

      pos += (ground - pos) / 10.0 * offset;
      camera_.set_camera(pos, up);
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
    while(true) {
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
  }

  inline void push_event(details::motion_event &&msg) noexcept {
    event_queue_.push(std::move(msg));
    triggered_.store(true, std::memory_order_release);
  }

  inline void reset() noexcept {
    key_pressed_.clear();
  }

  impl(std::function<void(u_ptr<scene_message> &&)> cb) noexcept
      : event_queue_{512},
        camera_{0, 0, glm::vec3{8000000.f, 0.f, 0.f}, glm::vec3{0.f, 0.f, 1.f}},
        update_callback_{cb},
        triggered_{false} {
    using namespace glm;
    auto event_pool = std::thread([&](){
      event_main();
    });
    event_pool.detach();
  }

private:
  utils::fifo<details::motion_event> event_queue_;
  class camera camera_;
  std::function<void(u_ptr<scene_message>&&)> update_callback_;
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
    : pimpl_{make_ptr_u<impl>([&](u_ptr<scene_message> &&msg){
      notify(std::move(msg));
    })} {}

scene_controller::~scene_controller() noexcept {}

void scene_controller::update(u_ptr<scene_message> &&msg) noexcept {
  assert(nullptr != pimpl_);

  pimpl_->receive_response();
  if (msg->camera != pimpl_->camera()) {
    notify(make_ptr_u<scene_message>(pimpl_->camera()));
  }
}

} // namespace scene

} // namespace esim
