#ifndef __ESIM_ESIM_SOURCE_ESIM_ENGINE_OPAQUE_H_
#define __ESIM_ESIM_SOURCE_ESIM_ENGINE_OPAQUE_H_

#include "core/fifo.h"
#include "core/utils.h"
#include "esim/esim_engine.h"
#include "esim_render_pipe.h"
#include "glapi/buffer.h"
#include "scene/atmosphere.h"
#include "scene/skysphere.h"
#include "scene/stellar.h"
#include "scene/surface_collections.h"
#include <atomic>
#include <glad/glad.h>
#include <scene/programs/blend_program.h>
#include <scene/programs/blur_program.h>
#include <thread>
#include <vector>

namespace esim {

class esim_engine::opaque {
private:
  enum class status : uint32_t {
    initialized = 0b1,
    rendering   = 0b10,
    pause       = 0b100,
    terminate   = 0b10000
  };

public:
  scene::frame_info frame_info() const noexcept;

  void render() noexcept;

  bool is_rendering() const noexcept;
  
  bool is_pause() const noexcept;
  
  bool is_terminated() const noexcept;

  void start() noexcept;

  void pause() noexcept;

  void resume() noexcept;

  void stop() noexcept;

  void terminate() noexcept;

  void push_frame_info(rptr<scene::frame_info> info) noexcept;

  bool poll_events() noexcept;

  opaque() noexcept;

  ~opaque() noexcept;

private:
  enums::raw<status> state(std::memory_order mo = std::memory_order_acquire) const noexcept;

  void prepare_normal_render_pipeline() noexcept;

private:
  std::atomic<enums::raw<status>> state_;
  core::fifo<scene::frame_info>   frame_info_queue_;
  scene::frame_info               frame_info_;
  uptr<esim_render_pipe>          pipeline_;

  /// render entities
  uptr<scene::stellar>            sun_entity_;
  uptr<scene::skysphere>          skysphere_entity_;
  uptr<scene::surface_collection> surface_entity_;
  uptr<scene::atmosphere>         atmosphere_entity_;

  ///frame buffer
  GLuint              hdr_fbo_, rbo_depth_;
  std::vector<GLuint> color_buffers_;
  std::vector<GLuint> pingpong_fbos_;
  std::vector<GLuint> pingpong_buffers_;
  gl::buffer<esim::details::blur_pos> quad_vbo_; 
};

} // namespace esim

#endif