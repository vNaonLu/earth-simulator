#ifndef __ESIM_MAIN_SOURCE_SCENE_SCNENE_H_
#define __ESIM_MAIN_SOURCE_SCENE_SCNENE_H_

#include "details/information.h"
#include <glad/glad.h>

namespace esim {

namespace scene {

class scene_entity {
public:
  virtual void render(const frame_info &info) noexcept = 0;
};

} // namespace scene

} // namespace esim

#endif