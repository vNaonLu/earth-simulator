#include "esim_render_pipe.h"
#include <cassert>

namespace esim {

void esim_render_pipe::clear() noexcept {
  head_ = 0;
}

void esim_render_pipe::render(const scene::frame_info &info) noexcept {
  auto it = pipeline_.begin(),
       end = pipeline_.begin() + head_;
  while (it != end) {
    (*it++)(info);
  }
}

void esim_render_pipe::push_render_event(render_func func) noexcept {
  assert(head_ < pipeline_.size());
  pipeline_[head_++] = func;
}

void esim_render_pipe::push_render_entity(rptr<scene::scene_entity> entity) noexcept {
  assert(head_ < pipeline_.size());
  pipeline_[head_++] = std::bind(&scene::scene_entity::render, entity,
                                 std::placeholders::_1);
}

esim_render_pipe::esim_render_pipe(size_t reserve_stage) noexcept
    : head_{0}, pipeline_(reserve_stage) {
}

esim_render_pipe::~esim_render_pipe() noexcept {}

} // namespace esim
