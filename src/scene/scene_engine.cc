#include "scene_engine.h"
#include "scene_controller.h"
#include "scene_message.h"
#include "scene_renderer.h"
#include <atomic>
#include <cassert>
#include <thread>
#include <utils/fifo.h>

namespace esim {

namespace scene {

class scene_engine::impl {
public:
  enum class render_state : uint32_t {
    normal  = 0b0,
    paused  = 0b1,

    stopped = static_cast<uint32_t>(1 << 31)
  };

  inline void update_viewport(uint32_t width, uint32_t height) noexcept {
    // assert(nullptr != ctrler_);
    // ctrler_->update_viewport(width, height);
  }

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

    state_.fetch_xor(to_raw(render_state::paused),
                     std::memory_order_release);
  }
  
  inline void stop_render() noexcept {

    state_.fetch_or(to_raw(render_state::stopped),
                    std::memory_order_release);
  }

  inline void render() noexcept {
    assert(nullptr != rderer_);
    consume_message();
    rderer_->render(camera_);
  }

  inline void zoom_in(double tick) noexcept {
  }
  
  inline void zoom_out(double tick) noexcept {
  }

  impl() noexcept
      : state_{to_raw(render_state::normal)},
        rderer_{make_ptr_u<scene_renderer>()},
        camera_{0, 0, glm::vec3{0.f, 0.0f, 0.3f}, glm::vec3{0.f, 0.f, 1.f}} {}

  ~impl() = default;

  inline void push_message(u_ptr<scene_message> msg) noexcept {
    resume_render();
    msg_queue_.push(std::move(msg));
  }

  inline void consume_message() noexcept {
    u_ptr<scene_message> msg;
    if (msg_queue_.try_pop(msg)) {
      camera_ = std::move(msg->camera);
    } else {
      pause_render();
    }
  }

private:
  utils::fifo<u_ptr<scene_message>> msg_queue_;
  std::atomic<raw<render_state>>    state_;
  // u_ptr<scene_controller>           ctrler_;
  u_ptr<scene_renderer>             rderer_;
  class camera                      camera_;
};

void scene_engine::update_viewport(uint32_t width, uint32_t height) noexcept {
  assert(nullptr != pimpl_);
  pimpl_->update_viewport(width, height);
}

void scene_engine::start(std::function<void()> before_rd,
                         std::function<void()> after_rd) noexcept {
  assert(nullptr != pimpl_);
  while (before_rd(), pimpl_->is_active()) {
    if (!pimpl_->is_pause()) {
      pimpl_->render();
      notify(make_ptr_u<scene_message>(pimpl_->camera()));
    } else {
      std::this_thread::yield();
    }

    after_rd();
  }
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

void scene_engine::zoom_in(double tick) noexcept {
  assert(nullptr != pimpl_);
  pimpl_->zoom_in(tick);
}

void scene_engine::zoom_out(double tick) noexcept {
  assert(nullptr != pimpl_);
  pimpl_->zoom_out(tick);
}

scene_engine::scene_engine() noexcept
    : pimpl_{make_ptr_u<impl>()} {}

scene_engine::~scene_engine() noexcept {}

void scene_engine::update(u_ptr<scene_message> &&msg) noexcept {
  assert(nullptr != pimpl_);

  if (msg->camera != pimpl_->camera()) {
    pimpl_->push_message(std::move(msg));
  }
}

} // namespace scene

} // namespace esim
