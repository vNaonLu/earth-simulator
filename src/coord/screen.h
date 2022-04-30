#ifndef __ESIM_COORD_SCREEN_H_
#define __ESIM_COORD_SCREEN_H_

#include <camera.h>
#include <glad/glad.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <limits>
#include <utility>

namespace esim {

namespace coord {

inline static glm::vec3 real_to_ndc(const camera &cmr, const glm::vec3 &pos) noexcept {
  using namespace glm;
  vec4 clip = cmr.get_vp() * vec4(pos, 1.f);

  return vec3{(clip.x / clip.w),
              (clip.y / clip.w),
              (clip.z / clip.w)};
}

inline static glm::ivec2 ndc_to_pixel(const camera &cmr, const glm::vec3 &ndc) noexcept {
  using namespace glm;
  const auto &viewport = cmr.viewport();

  return ivec2{(ndc.x * 0.5 + 0.5) * viewport.x,
               (ndc.y * 0.5 + 0.5) * viewport.y};
}

} // namespace coord

} // namespace esim

#endif