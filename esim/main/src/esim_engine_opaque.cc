#include "esim_engine_opaque.h"
#include <glad/glad.h>

namespace esim {

scene::frame_info esim_engine::opaque::frame_info() const noexcept {
  
  return frame_info_;
}

void esim_engine::opaque::render() noexcept {
  using namespace glm;
  pipeline_->render(frame_info_);
}

bool esim_engine::opaque::is_rendering() const noexcept {

  return state(std::memory_order_acquire) & status::rendering;
}

bool esim_engine::opaque::is_pause() const noexcept {

  return state(std::memory_order_acquire) & status::pause;
}

bool esim_engine::opaque::is_terminated() const noexcept {

  return state(std::memory_order_acquire) & status::terminate;
}

void esim_engine::opaque::start() noexcept {
  state_.fetch_or(enums::to_raw(status::rendering), std::memory_order_release);
}

void esim_engine::opaque::pause() noexcept {
  state_.fetch_or(enums::to_raw(status::pause), std::memory_order_release);
}

void esim_engine::opaque::resume() noexcept {
  state_.fetch_and(~enums::to_raw(status::pause), std::memory_order_release);
}

void esim_engine::opaque::stop() noexcept {
  state_.fetch_and(~enums::to_raw(status::rendering), std::memory_order_release);
}

void esim_engine::opaque::terminate() noexcept {
  state_.fetch_or(enums::to_raw(status::terminate), std::memory_order_release);
}

void esim_engine::opaque::push_frame_info(rptr<scene::frame_info> info) noexcept {
  resume();
  frame_info_queue_.try_push(*info);
}

bool esim_engine::opaque::poll_events() noexcept {
  size_t pop_count = 0;
  scene::frame_info info;
  while (frame_info_queue_.try_pop(info)) {
    ++pop_count;
  }

  if (pop_count > 0 && (frame_info_.camera != info.camera || frame_info_.sun != info.sun)) {
    frame_info_ = info;
    resume();
  }

  return pop_count != 0;
}

esim_engine::opaque::opaque() noexcept
    : state_{0}, frame_info_queue_{512},
      pipeline_{make_uptr<esim_render_pipe>(20)},
      surface_entity_ {make_uptr<scene::surface_collection>(33)} {
  pipeline_->push_render_event([](const scene::frame_info &info) {
    auto vp = info.camera.viewport();
    glViewport(0, 0, vp.x, vp.y);
    glEnable(GL_MULTISAMPLE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  });
  pipeline_->push_render_entity(surface_entity_.get());

  state_.fetch_or(enums::to_raw(status::initialized), std::memory_order_release);
}

esim_engine::opaque::~opaque() noexcept {
  stop();
  while (!(state(std::memory_order_acquire) & status::terminate)) {
    std::this_thread::yield();
  }
}

enums::raw<esim_engine::opaque::status>
esim_engine::opaque::state(std::memory_order mo) const noexcept {

  return state_.load(mo);
}

} // namespace esim
