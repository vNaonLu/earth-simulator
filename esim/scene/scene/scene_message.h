#ifndef __ESIM_SCENE_SCENE_SCENE_MESSAGE_H_
#define __ESIM_SCENE_SCENE_SCENE_MESSAGE_H_

#include "utils/camera.h"
#include "utils/utils.h"

namespace esim {

namespace scene {

/**
 * @brief The message block communicated by scene controller and scene engine
 * 
 */
struct scene_message {
  class camera camera;

  scene_message(const class camera &cmr) noexcept
      : camera{cmr} {}
};

} // namespace scene

} // namespace esim

#endif