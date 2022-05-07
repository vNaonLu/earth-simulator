#ifndef __ESIM_SCENE_LAYERS_ATOMOSPHERE_H_
#define __ESIM_SCENE_LAYERS_ATOMOSPHERE_H_

#include "scene/render/rendering_infos.h"
#include "utils/utils.h"

namespace esim {

namespace scene {

/**
 * @brief Atomosphere rendering class
 * 
 */
class atmosphere final {
public:
  /**
   * @brief Draw the atmosphere.
   * 
   */
  void draw(const rendering_infos &info) noexcept;

  /**
   * @brief Construct a new atomosphere object.
   * 
   */
  atmosphere() noexcept;

  /**
   * @brief Destroy the atomosphere object.
   * 
   */
  ~atmosphere() noexcept;

private:
  class impl;
  u_ptr<impl> pimpl_;
};

} // namespace scene

} // namespace esim

#endif