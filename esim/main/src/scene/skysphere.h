#ifndef __ESIM_MAIN_SOURCE_SCENE_SKYSPHERE_H_
#define __ESIM_MAIN_SOURCE_SCENE_SKYSPHERE_H_

#include "glapi/buffer.h"
#include "glapi/texture.h"
#include "programs/skysphere_program.h"
#include "scene/scene_entity.h"
#include <cstdint>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

namespace esim {

namespace scene {

class skysphere final : public scene::scene_entity {
public:
  void render(const frame_info &info) noexcept final;

  skysphere() noexcept;

  ~skysphere() noexcept;

private:
  std::vector<esim::details::skysphere_vertex> gen_vertex_buffer() noexcept;

  std::vector<uint32_t> gen_element_buffer() noexcept;

private:
  const glm::mat4x4                           GC_rotation_;
  glm::dvec3                                  offset_;
  gl::buffer<uint32_t>                        ebo_;
  gl::buffer<esim::details::skysphere_vertex> vbo_;
  gl::texture                                 skymap_;
};

} // namespace scene

} // namespace esim

#endif