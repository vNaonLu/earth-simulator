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
  auto view = cmr.view<float>();
  auto proj = cmr.project<float>();
  auto pos = proj * view * m * vec4(1.0, 0.0, 0.0, 1.0);
  pos.x /= pos.w;
  pos.y /= pos.w;

  auto program = program::sun_program::get();
  program->use();
  vbo_.bind();
  program->enable_position_pointer();
  program->update_viewport_uniform(static_cast<vec2>(cmr.viewport()));
  program->update_solar_location_uniform(pos);
  program->update_solar_color_uniform(vec4{1.0, 1.0, 1.0, 1.0f});
  program->update_pixel_size_uniform(0.5f);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(vbo_.size()));
}

stellar::stellar() noexcept : vbo_{GL_ARRAY_BUFFER, 1} {
  vbo_.bind_buffer(gen_vertex_buffer());
}

stellar::~stellar() noexcept {}

std::vector<esim::details::sun_vertex> stellar::gen_vertex_buffer() noexcept {
  using namespace glm;
  std::vector<esim::details::sun_vertex> buffer(4);
  auto it = buffer.begin();
  (*it++).pos = vec2(-1.0,  1.0);
  (*it++).pos = vec2(-1.0, -1.0);
  (*it++).pos = vec2( 1.0,  1.0);
  (*it++).pos = vec2( 1.0, -1.0);
  return buffer;
}

} // namespace scene

} // namespace esim
