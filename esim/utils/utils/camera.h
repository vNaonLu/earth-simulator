#ifndef __ESIM_UTILS_UTILS_CAMERA_H_
#define __ESIM_UTILS_UTILS_CAMERA_H_

#include "transform/wgs84.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

namespace esim {

class camera {
public:
  /**
   * @brief Obtain projection matrix
   * 
   * @return A projection matrix.
   */
  inline glm::mat4x4 projection() const noexcept {
    using namespace glm;
    const float aspect = viewport_.y == 0.0f ? 0.0f : (float)viewport_.x / (float)viewport_.y;
    
    return perspective(radians(45.f), aspect, 0.1f, 63781370.0f);
  }
  
  /**
   * @brief Obtain view matrix
   * 
   * @return A view matrix.
   */
  inline glm::mat4x4 view() const noexcept {
    using namespace glm;
    const static auto origin = vec3{0.f, 0.f, 0.f};

    return lookAt((vec3)ecef_, origin, up_);
  }

  inline const glm::ivec2 &viewport() const noexcept {

    return viewport_;
  }

  /**
   * @brief Obtain the camera position in ECEF coordinate
   * 
   * @return ECEF position
   */
  inline glm::dvec3 ecef() const noexcept {

    return ecef_;
  }
  
  /**
   * @brief Obtain up vector of camera
   * 
   * @return up-vector
   */
  inline glm::vec3 up() const noexcept {

    return up_;
  }

  /**
   * @brief Set the camera position and up-vector
   * 
   * @param pos specifies the ECEF position.
   * @param up specifies the oriented vector. 
   */
  inline void set_camera(const glm::dvec3 &pos, const glm::vec3 &up) noexcept {
    using namespace glm;
    up_ = normalize(up);
    ecef_ = pos;
  }

  /**
   * @brief Update the viewport height and width
   * 
   * @param width specifies the width of window.
   * @param height specifies the height of window.
   */
  inline void set_viewport(int width, int height) noexcept {
    viewport_.x = width;
    viewport_.y = height;
  }


  /**
   * @brief Construct a new camera object
   *
   * @param width specifies the width of viewport.
   * @param height specifies the height of viewport.
   * @param pos specifies the ECEF position.
   * @param up_vec specifies the up vector of camera.
   */
  camera(int width, int height,
         const glm::dvec3 &pos,
         const glm::vec3 &up_vec) noexcept
      : viewport_{width, height},
        up_{up_vec},
        ecef_{static_cast<glm::dvec3>(pos)} {}

  inline bool operator==(const camera &rhs) const noexcept {
    using namespace glm;
    return all(equal(viewport_, rhs.viewport_)) &&
           all(equal(ecef_, rhs.ecef_)) &&
           all(equal(up_, rhs.up_));
  }
  
  inline bool operator!=(const camera &rhs) const noexcept {

    return !(operator==(rhs));
  }

private:
  glm::ivec2 viewport_;
  glm::vec3  up_;
  glm::dvec3 ecef_;
};

} // namespace esim

#endif