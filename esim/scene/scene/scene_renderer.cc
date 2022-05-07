#include "scene_renderer.h"
#include "layers/atmosphere.h"
#include "layers/debug_layer.h"
#include "layers/solar.h"
#include "layers/surface_layer.h"
#include <glad/glad.h>

namespace esim {

namespace scene {

class scene_renderer::impl {
public:
  inline void render(const rendering_infos &info) noexcept {
    auto &cmr = info.camera;
    auto vp = cmr.viewport();
    glEnable(GL_MULTISAMPLE);
    glViewport(0, 0, vp.x, vp.y);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    surface_layer_->draw(info);
    solar_->draw(info);
    atmosphere_layer_->draw(info);
    // surface_layer_->draw_bounding_box(cmr);
    // debuger_layer_->draw(cmr);
  }

  impl() noexcept
      : atmosphere_layer_{make_ptr_u<atmosphere>()},
        surface_layer_{make_ptr_u<surface_layer>(33)},
        debuger_layer_{make_ptr_u<debug_layer>()},
        solar_{make_ptr_u<solar>()} {}

private:
  u_ptr<atmosphere> atmosphere_layer_;
  u_ptr<surface_layer> surface_layer_;
  u_ptr<debug_layer> debuger_layer_;
  u_ptr<solar> solar_;
};

void scene_renderer::render(const rendering_infos &info) noexcept {
  assert(nullptr != pimpl_);
  pimpl_->render(info);
}

scene_renderer::scene_renderer() noexcept
    : pimpl_{make_ptr_u<impl>()} {}

    
scene_renderer::~scene_renderer() noexcept {}

} // namespace scene

} // namespace esim
