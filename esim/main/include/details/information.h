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
  bool         debug_show_scene;
  bool         debug_show_light;
};

inline bool operator==(const frame_info &lhs, const frame_info &rhs) {
  
  return (lhs.camera == rhs.camera) &&
         (lhs.sun == rhs.sun) &&
         (lhs.debug_show_light == rhs.debug_show_light) &&
         (lhs.debug_show_scene == rhs.debug_show_scene);
}

inline bool operator!=(const frame_info &lhs, const frame_info &rhs) {
  
  return !operator==(lhs, rhs);
}

} // namespace scene

} // namespace esim

#endif