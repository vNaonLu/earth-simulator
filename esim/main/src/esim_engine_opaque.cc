#include "esim_engine_opaque.h"
#include "core/transform.h"
#include <glad/glad.h>

namespace esim {

scene::frame_info esim_engine::opaque::frame_info() const noexcept {
  
  return frame_info_;
}

void esim_engine::opaque::render() noexcept {
  using namespace glm;
  auto &cmr = frame_info_.camera;
  auto &sun = frame_info_.sun;

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /// draw as usual
  glBindFramebuffer(GL_FRAMEBUFFER, hdr_fbo_);

  auto vp = frame_info_.camera.viewport();
  glViewport(0, 0, vp.x, vp.y);
  glEnable(GL_MULTISAMPLE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
  glDepthFunc(GL_LEQUAL);

  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CW);
  skysphere_entity_->render(frame_info_);
  glFrontFace(GL_CCW);

  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
  sun_entity_->render(frame_info_);

  glDepthMask(GL_TRUE);
  glFrontFace(GL_CCW);
  atmosphere_entity_->render(frame_info_);

  glFrontFace(GL_CCW);
  surface_entity_->render(frame_info_);
  // surface_entity_->render_bounding_box(frame_info_);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  /// blend
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  auto blend_prog = program::blend_program::get();
  blend_prog->use();
  quad_vbo_.bind();
  blend_prog->enable_position_pointer();

  auto m = translate(mat4x4{1.0f}, -cmr.pos<float>());
  m = scale(m, vec3{astron::au<float>()});
  m = sun.rotate_to_solar_direction(m);
  auto view = cmr.view<float>();
  auto proj = cmr.project<float>();
  auto sun_ndc = proj * view * m * vec4(1.0, 0.0, 0.0, 1.0);
  sun_ndc.x /= sun_ndc.w;
  sun_ndc.y /= sun_ndc.w;

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, color_buffers_[0]);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, color_buffers_[1]);
  noise_.bind(0, 2);
  blend_prog->update_enable_scattering_uniform(1);
  blend_prog->update_gamma_uniform(1.0f / 2.2f);
  blend_prog->update_exposure_uniform(2.0f);
  blend_prog->update_ndc_sun_uniform(static_cast<vec4>(sun_ndc));
  blend_prog->update_resolution_uniform(static_cast<vec2>(cmr.viewport()));
  blend_prog->update_dither_resolution_uniform(static_cast<vec2>(noise_.resolution()));
  glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(quad_vbo_.size()));

  /// debug
  render_framebuffer();
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

  if (pop_count > 0 && frame_info_ != info) {
    frame_info_ = info;
    resume();
  }

  return pop_count != 0;
}

esim_engine::opaque::opaque() noexcept
    : state_{0}, frame_info_queue_{512},
      pipeline_{make_uptr<esim_render_pipe>(20)},
      sun_entity_{make_uptr<scene::stellar>()},
      skysphere_entity_{make_uptr<scene::skysphere>()},
      surface_entity_{make_uptr<scene::surface_collection>(33)},
      atmosphere_entity_{make_uptr<scene::atmosphere>()},
      color_buffers_(2), quad_vbo_{GL_ARRAY_BUFFER} {
  using namespace glm;

  assert(noise_.load("assets/img/noise.png"));

  glGenFramebuffers(1, &hdr_fbo_);
  glBindFramebuffer(GL_FRAMEBUFFER, hdr_fbo_);
  glGenRenderbuffers(1, &rbo_depth_);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth_);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1080, 1080);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  glGenTextures(2, color_buffers_.data());
  for (GLuint i = 0; i < 2; ++i) {
    glBindTexture(GL_TEXTURE_2D, color_buffers_[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 1080, 1080, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, color_buffers_[i], 0);
  }
  GLuint attachments[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
  glDrawBuffers(2, attachments);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth_);
  assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  quad_vbo_.bind_buffer(std::vector<details::screen_vertex>{details::screen_vertex{{-1.0, 1.0}},
                                                            details::screen_vertex{{-1.0, -1.0}},
                                                            details::screen_vertex{{1.0, 1.0}},
                                                            details::screen_vertex{{1.0, -1.0}}});

  // prepare_normal_render_pipeline();
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

void esim_engine::opaque::prepare_normal_render_pipeline() noexcept {
  pipeline_->push_render_event(
      [](const scene::frame_info &info) {
        auto vp = info.camera.viewport();
        glViewport(0, 0, vp.x, vp.y);
        glEnable(GL_MULTISAMPLE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
      });
  pipeline_->push_render_entity(skysphere_entity_.get());
  pipeline_->push_render_event(
      []([[maybe_unused]] const scene::frame_info &info) {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
      });
  pipeline_->push_render_entity(sun_entity_.get());
  pipeline_->push_render_event(
      []([[maybe_unused]] const scene::frame_info &info) {
        glDepthMask(GL_TRUE);
      });
  pipeline_->push_render_entity(surface_entity_.get());
  pipeline_->push_render_entity(atmosphere_entity_.get());
}

void esim_engine::opaque::render_framebuffer() noexcept {
  using namespace glm;
  constexpr static ivec2 rez = {100, 100};
  GLsizei frame_count = 0;
  auto screen_prog = program::screen_program::get();
  screen_prog->use();
  quad_vbo_.bind();
  screen_prog->enable_position_pointer();
  screen_prog->update_alpha_uniform(0.5f);
  if (frame_info_.debug_show_scene) {
    glViewport(rez.x * (frame_count / 4), 
               rez.y * (frame_count % 4), rez.x, rez.y);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, color_buffers_[0]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(quad_vbo_.size()));
    ++frame_count;
  }

  if(frame_info_.debug_show_light) {
    glViewport(rez.x * (frame_count / 4), 
               rez.y * (frame_count % 4), rez.x, rez.y);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, color_buffers_[1]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(quad_vbo_.size()));
    ++frame_count;
  }
}

} // namespace esim
