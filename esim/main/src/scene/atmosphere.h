#ifndef __ESIM_MAIN_SOURCE_SCENE_ATMOSPHERE_H_
#define __ESIM_MAIN_SOURCE_SCENE_ATMOSPHERE_H_

#include "glapi/buffer.h"
#include "programs/atmosphere_program.h"
#include "scene/scene_entity.h"
#include <cstdint>
#include <glm/vec4.hpp>

namespace esim {

namespace scene {

class atmosphere final : public scene::scene_entity {
public:
  void render(const frame_info &info) noexcept;

  atmosphere() noexcept;

  ~atmosphere() noexcept;

private:
  std::vector<esim::details::atmosphere_vertex> gen_vertex_buffer() noexcept;

  std::vector<uint32_t> gen_element_buffer() noexcept;

private:
  glm::dvec3                                   offset_;
  gl::buffer<uint32_t>                         ebo_;
  gl::buffer<esim::details::atmosphere_vertex> vbo_;
};

} // namespace scene

} // namespace esim

#endif