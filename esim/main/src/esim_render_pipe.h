#ifndef __ESIM_MAIN_SOURCE_ESIM_RENDER_PIPE_H_
#define __ESIM_MAIN_SOURCE_ESIM_RENDER_PIPE_H_

#include "core/utils.h"
#include "scene/scene_entity.h"
#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <vector>

namespace esim {

class esim_render_pipe final {
public:
  void render(const scene::frame_info &info) noexcept;

  void push_entity(rptr<scene::scene_entity> entity) noexcept;

  void initialize_frame(GLbitfield clear_mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
                        glm::vec4 clear_color = glm::vec4{0.0f},
                        bool enable_multisamplet = true) noexcept;

  esim_render_pipe(size_t reserve_stage = 100) noexcept;

  ~esim_render_pipe() noexcept;

private:
  size_t head_;
  std::vector<rptr<scene::scene_entity>> pipeline_;
};

} // namespace esim

#endif