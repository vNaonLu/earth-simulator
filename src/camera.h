#ifndef __ESIM_CAMERA_H_
#define __ESIM_CAMERA_H_

#include <coord/transform.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

namespace esim {

class camera {
public:
  /// TODO: fifo atomic get position

  /**
   * @brief Obtain view and projection matrix
   * 
   * @return A VP matrix.
   */
  inline glm::mat4x4 get_vp() const noexcept {
    using namespace glm;
    const static auto origin = vec3{0.f, 0.f, 0.f};
    auto res = perspective(radians(45.f), (float)viewport_.x / (float)viewport_.y, 0.1f, 100.0f);
    res *= lookAt((vec3)ecef_, origin, up_);

    return res;
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
   * @brief Obtain the camera position in LLA coordinate
   * 
   * @return lat-lon-alt position
   */
  inline glm::vec3 lla() const noexcept {

    return lla_;
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
   * @param lla specifies the LLA position.
   * @param up specifies the oriented vector. 
   */
  inline void set_camera(const glm::vec3 &lla, const glm::vec3 &up) noexcept {
    using namespace glm;
    lla_ = lla;
    up_ = normalize(up);
    ecef_ = (dvec3)lla;
    coord::geodetic_to_ecef<coord::TEST>(ecef_.x, ecef_.y, ecef_.z);
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
   * @param lla_pos specifies the height of viewport.
   * @param lla_pos specifies the position in degrees latitude, longitude and altitude.
   */
  camera(int width, int height,
         const glm::vec3 &lla_pos,
         const glm::vec3 &up_vec) noexcept
      : viewport_{width, height},
        lla_{lla_pos},
        up_{up_vec},
        ecef_{(glm::dvec3)lla_pos} {
    coord::geodetic_to_ecef<coord::TEST>(ecef_.x, ecef_.y, ecef_.z);
  }

private:
  glm::ivec2 viewport_;
  glm::vec3  lla_;
  glm::vec3  up_;
  glm::dvec3 ecef_;
};

} // namespace esim

#endif