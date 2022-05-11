#include "scene/stellar.h"
#include "core/transform.h"

namespace esim {

namespace scene {

void stellar::render(const frame_info &info) noexcept {
  using namespace glm;
  auto &cmr = info.camera;
  auto &sun = info.sun;
  auto m = translate(mat4x4{1.0f}, -cmr.pos<float>());
  m = scale(m, vec3{astron::au<float>()});
  m = sun.rotate_to_solar_direction(m);

  auto program = program::sun_program::get();
  program->use();
  vbo_.bind();
  program->enable_position_pointer();
  program->update_viewport_uniform(static_cast<vec2>(cmr.viewport()));
  program->update_project_uniform(cmr.project<float>());
  program->update_view_uniform(cmr.view<float>());
  program->update_model_uniform(m);
  program->update_pixel_size_uniform(10.0f);
  program->update_solar_color_uniform(vec4{1.0f, 1.0f, 1.0f, 1.0f});
  glDrawArrays(GL_TRIANGLE_FAN, 1, static_cast<GLsizei>(vbo_.size() - 1));
}

stellar::stellar() noexcept : vbo_{GL_ARRAY_BUFFER, 1} {
  vbo_.bind_buffer(gen_vertex_buffer());
}

stellar::~stellar() noexcept {}

std::vector<esim::details::sun_vertex> stellar::gen_vertex_buffer() noexcept {
  using namespace glm;
  constexpr static size_t sun_details = 8;
  std::vector<esim::details::sun_vertex> buffer(sun_details + 2 + 1);
  auto it = buffer.begin();
  (*it++).pos = vec3(0.0f);
  (*it++).pos = vec3(1.0f, 0.0f, 0.0f);

  float stride = 360.0f / static_cast<float>(sun_details);
  for (size_t i=0; i<=sun_details; ++i) {
    float rad = radians(stride * i);
    (*it++).pos = vec3(1.0f, cos(rad), sin(rad));
  }

  return buffer;
}

} // namespace scene

} // namespace esim
