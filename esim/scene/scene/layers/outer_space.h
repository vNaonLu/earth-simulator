#ifndef __ESIM_SCENE_LAYERS_OUTER_SPACE_H_
#define __ESIM_SCENE_LAYERS_OUTER_SPACE_H_

#include "scene/render/rendering_infos.h"
#include "utils/utils.h"

namespace esim {

namespace scene {

/**
 * @brief A skysphere outer space scene.
 * 
 */
class outer_space final {
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
  outer_space() noexcept;

  /**
   * @brief Destroy the atomosphere object.
   * 
   */
  ~outer_space() noexcept;

private:
  class impl;
  u_ptr<impl> pimpl_;
};

} // namespace scene

} // namespace esim

#endif