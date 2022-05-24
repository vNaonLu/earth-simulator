#include "esim/esim_engine.h"
#include "esim_engine_opaque.h"

#include <iostream>

namespace esim {

void esim_engine::start(std::function<void()> before_render,
                        std::function<void()> after_render) noexcept {
  assert(nullptr != opaque_);
  opaque_->start();
  while (before_render(), opaque_->is_rendering()) {
    opaque_->poll_events();

    if (!opaque_->is_pause()) {
      opaque_->render();
      after_render();
    }
    opaque_->after_render();

    auto info = opaque_->frame_info();
    notify(&info);
  }
  opaque_->terminate();
}

void esim_engine::stop() noexcept {
  assert(nullptr != opaque_);
  opaque_->stop();
}

void esim_engine::render() noexcept {
  assert(nullptr != opaque_);
  opaque_->poll_events();
  opaque_->render();
  auto info = opaque_->frame_info();
  notify(&info);
}

esim_engine::esim_engine() noexcept
    : opaque_{make_uptr<opaque>()} {}

esim_engine::~esim_engine() noexcept {
  assert(nullptr != opaque_);
  opaque_->stop();
}

void esim_engine::update(rptr<void> msg) noexcept {
  assert(nullptr != opaque_);
  auto info = reinterpret_cast<rptr<scene::frame_info>>(msg);
  opaque_->push_frame_info(info);
}

} // namespace esim
