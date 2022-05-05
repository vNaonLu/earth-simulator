#ifndef __ESIM_SCENE_RENDER_RENDERING_INFOS_H_
#define __ESIM_SCENE_RENDER_RENDERING_INFOS_H_

#include "utils/camera.h"
#include "utils/sun.h"

namespace esim {

namespace scene {

/**
 * @brief Package of rendering information
 * 
 */
struct rendering_infos {
  class camera camera;
  class sun    sun;
};

} // namespace scene

} // namespace esim

#endif