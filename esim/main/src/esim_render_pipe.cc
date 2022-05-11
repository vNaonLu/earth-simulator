#include "esim_render_pipe.h"
#include <cassert>

namespace esim {

namespace details {

struct frame_initializer : public scene::scene_entity {
  bool       enable_multisample;
  glm::vec4  clear_color;
  GLbitfield clear_mask;

  void render(const scene::frame_info &info) noexcept final {
    if (enable_multisample) {
      glEnable(GL_MULTISAMPLE);
    }
    auto vp = info.camera.viewport(); 
    glViewport(0, 0, vp.x, vp.y);
    glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
    glClear(clear_mask);
  }
};

} // namespace details


void esim_render_pipe::render(const scene::frame_info &info) noexcept {
  auto it = pipeline_.begin(),
       end = pipeline_.begin() + head_;
  while (it != end) {
    (*it++)->render(info);
  }

  head_ = 0;
}

void esim_render_pipe::push_entity(rptr<scene::scene_entity> entity) noexcept {
  assert(head_ < pipeline_.size());
  pipeline_[head_++] = entity;
}

void esim_render_pipe::initialize_frame(GLbitfield clear_mask, glm::vec4 clear_color,
                                        bool enable_multisample) noexcept {
  static struct details::frame_initializer frame_initializer;
  assert(head_ < pipeline_.size());
  frame_initializer.clear_mask = clear_mask;
  frame_initializer.clear_color = clear_color;
  frame_initializer.enable_multisample = enable_multisample;
  pipeline_[head_++] = &frame_initializer;
}

esim_render_pipe::esim_render_pipe(size_t reserve_stage) noexcept
    : head_{0}, pipeline_(reserve_stage) {
}

esim_render_pipe::~esim_render_pipe() noexcept {}

} // namespace esim
