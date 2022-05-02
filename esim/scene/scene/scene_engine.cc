#include "scene_engine.h"
#include "scene_controller.h"
#include "scene_message.h"
#include "scene_renderer.h"
#include "utils/fifo.h"
#include <atomic>
#include <cassert>
#include <thread>

namespace esim {

namespace scene {

class scene_engine::impl {
public:
  enum class render_state : uint32_t {
    normal  = 0b0,
    paused  = 0b1,

    stopped = static_cast<uint32_t>(1 << 31)
  };

  inline class camera camera() const noexcept {
    return camera_;
  }

  inline bool is_active() const noexcept {
    using namespace esim::enums;
    return !(state_.load(std::memory_order_acquire) & render_state::stopped);
  }
  
  inline bool is_pause() const noexcept {
    using namespace esim::enums;
    return state_.load(std::memory_order_acquire) & render_state::paused;
  }

  inline void pause_render() noexcept {

    state_.fetch_or(to_raw(render_state::paused),
                    std::memory_order_release);
  }
  
  inline void resume_render() noexcept {

    state_.fetch_and(~to_raw(render_state::paused),
                     std::memory_order_release);
  }
  
  inline void stop_render() noexcept {

    state_.fetch_or(to_raw(render_state::stopped),
                    std::memory_order_release);
  }

  inline void render() noexcept {
    assert(nullptr != rderer_);
    // std::cout << "draw " << glm::to_string(camera_.ecef()) << std::endl;
    rderer_->render(camera_);
  }

  impl() noexcept
      : msg_queue_{512},
        state_{to_raw(render_state::normal)},
        rderer_{make_ptr_u<scene_renderer>()},
        camera_{0, 0, glm::vec3{0.f, 0.0f, 0.0f}, glm::vec3{0.f, 0.f, 1.f}} {}

  ~impl() = default;

  inline void push_event(u_ptr<scene_message> msg) noexcept {
    resume_render();
    msg_queue_.try_push(std::move(msg));
  }

  inline void poll_events() noexcept {
    u_ptr<scene_message> msg;
    while (msg_queue_.try_pop(msg));
    if (msg && camera_ != msg->camera) {
      camera_ = std::move(msg->camera);
      resume_render();
    } else {
      /// FIX: fix it
      // pause_render();
    }
  }

private:
  utils::fifo<u_ptr<scene_message>> msg_queue_;
  std::atomic<raw<render_state>>    state_;
  u_ptr<scene_renderer>             rderer_;
  class camera                      camera_;
};

void scene_engine::start(std::function<void()> before_rd,
                         std::function<void()> after_rd) noexcept {
  assert(nullptr != pimpl_);
  while (before_rd(), pimpl_->is_active()) {
    pimpl_->poll_events();

    if (!pimpl_->is_pause()) {
      pimpl_->render();
      /// notify observer the last camera position
      notify(make_ptr_u<scene_message>(pimpl_->camera()));
      after_rd();
    }
  }
}

void scene_engine::render() noexcept {
  assert(nullptr != pimpl_);
  pimpl_->poll_events();
  pimpl_->render();
  /// notify observer the last camera position
  notify(make_ptr_u<scene_message>(pimpl_->camera()));
}

void scene_engine::pause() noexcept {
  assert(nullptr != pimpl_);
  pimpl_->pause_render();
}

void scene_engine::resume() noexcept {
  assert(nullptr != pimpl_);
  pimpl_->resume_render();
}

void scene_engine::stop() noexcept {
  assert(nullptr != pimpl_);
  pimpl_->stop_render();
}

scene_engine::scene_engine() noexcept
    : pimpl_{make_ptr_u<impl>()} {}

scene_engine::~scene_engine() noexcept {}

void scene_engine::update(u_ptr<scene_message> &&msg) noexcept {
  assert(nullptr != pimpl_);
  pimpl_->push_event(std::move(msg));
}

} // namespace scene

} // namespace esim
