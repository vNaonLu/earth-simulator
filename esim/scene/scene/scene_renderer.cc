#include "scene_renderer.h"
#include "layers/debug_layer.h"
#include "layers/surface_layer.h"
#include <glad/glad.h>

namespace esim {

namespace scene {

class scene_renderer::impl {
public:
  inline void render(const camera &cmr) noexcept {
    auto vp = cmr.viewport();

    glViewport(0, 0, vp.x, vp.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);
    
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    surface_layer_->draw(cmr);
    // debuger_layer_->draw(cmr);
    // surface_layer_->draw_bounding_box(cmr);
  }

  impl() noexcept
      : surface_layer_{make_ptr_u<surface_layer>(15)},
        debuger_layer_{make_ptr_u<debug_layer>()} {}

private:
  u_ptr<surface_layer> surface_layer_;
  u_ptr<debug_layer> debuger_layer_;
};

void scene_renderer::render(const camera &cmr) noexcept {
  assert(nullptr != pimpl_);
  pimpl_->render(cmr);
}

scene_renderer::scene_renderer() noexcept
    : pimpl_{make_ptr_u<impl>()} {}

    
scene_renderer::~scene_renderer() noexcept {}

} // namespace scene

} // namespace esim
