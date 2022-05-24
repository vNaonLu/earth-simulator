#ifndef __ESIM_MAIN_SOURCE_ESIM_RENDER_PIPE_H_
#define __ESIM_MAIN_SOURCE_ESIM_RENDER_PIPE_H_

#include "core/utils.h"
#include "scene/scene_entity.h"
#include <functional>
#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <vector>

namespace esim {

class esim_render_pipe final {
public:
  typedef std::function<void(scene::frame_info)> render_func;

  void clear() noexcept;

  void render(const scene::frame_info &info) noexcept;

  void push_render_event(render_func func) noexcept;

  void push_render_entity(rptr<scene::scene_entity> entity) noexcept;

  esim_render_pipe(size_t reserve_stage = 100) noexcept;

  ~esim_render_pipe() noexcept;

private:
  size_t                   head_;
  std::vector<render_func> pipeline_;
};

} // namespace esim

#endif