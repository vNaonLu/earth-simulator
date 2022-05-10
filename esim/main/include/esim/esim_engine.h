#ifndef __ESIM_MAIN_ESIM_ESIM_ENGINE_H_
#define __ESIM_MAIN_ESIM_ESIM_ENGINE_H_

#include "core/observer.h"
#include "core/publisher.h"
#include "core/utils.h"
#include "details/information.h"
#include "details/protocol.h"
#include <functional>

namespace esim {

class esim_engine final : public core::observer,
                          public core::publisher {
public:
  /**
   * @brief Start drawing the scene loop.
   * 
   * @param before_render specifies the callback before rendering.
   * @param after_render specifies the callback after rendering.
   * @return true if the scene open successfully.
   */
  void start(std::function<void()> before_render,
             std::function<void()> after_render) noexcept;

  /**
   * @brief Stop the drawing loop.
   * 
   */
  void stop() noexcept;

  /**
   * @brief Draw a frame.
   * 
   */
  void render() noexcept;

  /**
   * @brief Construct a new esim engine object.
   * 
   */
  esim_engine() noexcept;

  /**
   * @brief Destroy the esim engine object.
   * 
   */
  ~esim_engine() noexcept;

protected:
  void update(rptr<void> msg) noexcept final;

private:
  class opaque;
  uptr<opaque> opaque_;
};

} // namespace esim

#endif