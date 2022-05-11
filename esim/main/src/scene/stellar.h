#ifndef __ESIM_MAIN_SOURCE_SCENE_STELLAR_H_
#define __ESIM_MAIN_SOURCE_SCENE_STELLAR_H_

#include "glapi/buffer.h"
#include "programs/sun_program.h"
#include "scene_entity.h"

namespace esim {

namespace scene {

class stellar final : public scene_entity {
public:
  void render(const frame_info &info) noexcept final;

  stellar() noexcept;

  ~stellar() noexcept;

private:
  std::vector<esim::details::sun_vertex> gen_vertex_buffer() noexcept;

private:
  gl::buffer<esim::details::sun_vertex> vbo_;
};

} // namespace scene

} // namespace esim

#endif