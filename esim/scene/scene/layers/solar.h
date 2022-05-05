#ifndef __ESIM_SCENE_LAYER_SOLAR_H_
#define __ESIM_SCENE_LAYER_SOLAR_H_

#include "scene/render/rendering_infos.h"
#include "utils/camera.h"
#include "utils/utils.h"

namespace esim {

namespace scene {

class solar {
public:
  /**
   * @brief Draw the solar
   * 
   * @param info specifies the rendering information.
   */
  void draw(const rendering_infos &info) noexcept;

  /**
   * @brief Construct a new solar object
   * 
   */
  solar() noexcept;

  /**
   * @brief Destroy the solar object
   * 
   */
  ~solar() noexcept;

private:
  class impl;
  u_ptr<impl> pimpl_;
};

} // namespace scene

} // namespace esim

#endif