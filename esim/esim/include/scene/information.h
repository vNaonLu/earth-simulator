#ifndef __ESIM_ESIM_SCENE_INFORMATION_H_
#define __ESIM_ESIM_SCENE_INFORMATION_H_

#include "camera.h"
#include "sun.h"

namespace esim {

namespace scene {

/**
 * @brief The information for a frame rendering.
 *
 */
struct frame_info {
  class camera camera;
  class sun    sun;
};

} // namespace scene

} // namespace esim

#endif