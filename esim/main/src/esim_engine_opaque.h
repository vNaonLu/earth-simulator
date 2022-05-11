#ifndef __ESIM_ESIM_SOURCE_ESIM_ENGINE_OPAQUE_H_
#define __ESIM_ESIM_SOURCE_ESIM_ENGINE_OPAQUE_H_

#include "core/fifo.h"
#include "core/utils.h"
#include "esim/esim_engine.h"
#include "scene/surface_collections.h"
#include <atomic>
#include <glad/glad.h>
#include <thread>

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

private:
  std::atomic<enums::raw<status>> state_;
  core::fifo<scene::frame_info>   frame_info_queue_;
  scene::frame_info               frame_info_;

  /// render entities
  uptr<scene::surface_collection> surface_entity_;
};

} // namespace esim

#endif