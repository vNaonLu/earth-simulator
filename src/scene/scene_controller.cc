#include "scene_controller.h"

namespace esim {

namespace scene {

class scene_controller::impl {
public:
  inline const class camera &camera() const noexcept {

    return camera_;
  }

  inline void update_viewport(uint32_t width, uint32_t height) noexcept {

    camera_.set_viewport(width, height);
  }

  inline void zoom_in(double tick) noexcept {
    auto pos = camera_.lla();
    auto up = camera_.up();

    pos.z -= static_cast<float>(pos.z * tick / 10.f);
    camera_.set_camera(pos, up);
  }
  
  inline void zoom_out(double tick) noexcept {
    auto pos = camera_.lla();
    auto up = camera_.up();

    pos.z += static_cast<float>(pos.z * tick / 10.f);
    camera_.set_camera(pos, up);
  }

  impl() noexcept
      : camera_{0, 0, glm::vec3{0.f, 0.f, 3.f}, glm::vec3{0.f, 0.f, 1.f}} {}

private:
  class camera camera_;

};

const class camera &scene_controller::camera() const noexcept {
  assert(nullptr != pimpl_);
  return pimpl_->camera();
}

void scene_controller::update_viewport(uint32_t width, uint32_t height) noexcept{
  assert(nullptr != pimpl_);
  pimpl_->update_viewport(width, height);
}

void scene_controller::zoom_in(double tick) noexcept {
  assert(nullptr != pimpl_);
  return pimpl_->zoom_in(tick);
}

void scene_controller::zoom_out(double tick) noexcept {
  assert(nullptr != pimpl_);
  return pimpl_->zoom_out(tick);
}

scene_controller::scene_controller() noexcept
    : pimpl_{make_ptr_u<impl>()} {}

scene_controller::~scene_controller() noexcept {}

} // namespace scene

} // namespace esim
