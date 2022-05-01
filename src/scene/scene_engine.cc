#include "scene_engine.h"
#include "scene_controller.h"
#include "scene_renderer.h"
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

  inline void update_viewport(uint32_t width, uint32_t height) noexcept {
    assert(nullptr != ctrler_);
    ctrler_->update_viewport(width, height);
  }

  inline class camera camera() const noexcept {
    assert(nullptr != ctrler_);
    return ctrler_->camera();
  }

  inline bool is_active() const noexcept {
    using namespace esim::enums;
    return state_.load(std::memory_order_acquire) ^ render_state::stopped;
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
    auto cmr = camera();
    rderer_->render(cmr);
  }

  inline void zoom_in(double tick) noexcept {
    assert(nullptr != ctrler_);
    ctrler_->zoom_in(tick);
  }
  
  inline void zoom_out(double tick) noexcept {
    assert(nullptr != ctrler_);
    ctrler_->zoom_out(tick);
  }


  impl() noexcept
      : state_{to_raw(render_state::normal)},
        ctrler_{make_ptr_u<scene_controller>()},
        rderer_{make_ptr_u<scene_renderer>()} {}

  ~impl() = default;

private:
  std::atomic<raw<render_state>> state_;
  u_ptr<scene_controller>        ctrler_;
  u_ptr<scene_renderer>          rderer_;
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

} // namespace scene

} // namespace esim
