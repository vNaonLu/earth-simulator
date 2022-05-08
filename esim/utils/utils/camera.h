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
  template <typename type = float>
  inline glm::mat<4, 4, type> projection() const noexcept {
    using namespace glm;
    const type aspect = viewport_.y == 0.0f ? 0.0f : (type)viewport_.x / (type)viewport_.y;

    return perspective(static_cast<type>(radians(45.f)), aspect,
                       static_cast<type>(near_far_.x),
                       static_cast<type>(near_far_.y));
  }

  /**
   * @brief Obtain skysphere perspective matrix
   * 
   * @return A projection matrix.
   */
  inline glm::mat4x4 skysphere_projection() const noexcept {
    using namespace glm;
    const float aspect = viewport_.y == 0.0f ? 0.0f : (float)viewport_.x / (float)viewport_.y;
    
    return perspective(radians(45.f), aspect, 0.0f, sqrt(2.0f));
  }
  
  /**
   * @brief Obtain view matrix
   * 
   * @return A view matrix.
   */
  inline glm::mat4x4 view() const noexcept {
    using namespace glm;
    constexpr static vec3 pos{0.f, 0.f, 0.f};

    return lookAt(pos, (vec3)dir_, (vec3)up_);
  }
  
  /**
   * @brief Obtain model matrix includes ONLY model translation
   * 
   * @return A model matrix
   */
  inline glm::mat4x4 model(glm::dvec3 offset) const noexcept {
    using namespace glm;
    constexpr static mat4x4 ident{1.0f};

    return translate(ident, static_cast<vec3>(offset - ecef_));
  }

  /**
   * @brief Obtain the viewport information
   * 
   * @return ivec2 with x = width and y = heigth
   */
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
   * @brief Obtain view vector of camera
   * 
   * @return view-vector
   */
  inline glm::vec3 dir() const noexcept {

    return dir_;
  }

  /**
   * @brief Obtain the camera information contains pos, dir and up
   * 
   * @return a tuple of pos, dir and up vector
   */
  inline std::tuple<glm::dvec3, glm::dvec3, glm::dvec3> information() const noexcept {
    
    return std::make_tuple(ecef_, dir_, up_);
  }

  /**
   * @brief Set the camera position and up-vector
   * 
   * @param pos specifies the ECEF position.
   * @param dir specifies the direction of view. 
   * @param up specifies the oriented vector. 
   */
  inline void set_camera(const glm::dvec3 &pos, const glm::dvec3 &dir, const glm::dvec3 &up) noexcept {
    using namespace glm;
    ecef_ = pos;
    dir_ = normalize(dir);
    up_ = normalize(up);
    if (!any(isinf(pos)) && !any(isinf(dir)) && !any(isinf(up))) {
      if (double dotprod = dot(dir_, up_); dotprod != 0.0f) {
        /// adjust up direction
        /// to make sure there is 90 degrees between dir and up
        up_ = normalize(up_ - dir_ * dotprod);
      }
      calculate_near_far();
    } else {
      assert(false);
      reset_information();
    }
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
   * @brief Reset camera to the default status
   * 
   */
  inline void reset_information() noexcept {
    using namespace glm;
    up_ = dvec3{0.0f, 0.0f, 1.0f};
    dir_ = dvec3{-1.0f, 0.0f, 0.0f};
    ecef_ = dvec3{trans::WGS84_A * 3.0, 0.0f, 0.0f};
    calculate_near_far();
  }

  /**
   * @brief Default camera constructor
   * 
   */
  camera() noexcept : viewport_{800, 600},
                      near_far_{0.0f, 0.0f},
                      up_{0.0f, 0.0f, 1.0f},
                      dir_{-1.0f, 0.0f, 0.0f},
                      ecef_{trans::WGS84_A * 3.0, 0.0f, 0.0f} {
    calculate_near_far();
  }

  /**
   * @brief Copy a new camera object
   * 
   */
  camera(const camera &cmr) noexcept : viewport_{cmr.viewport_},
                                       near_far_{cmr.near_far_},
                                       up_{cmr.up_},
                                       dir_{cmr.dir_},
                                       ecef_{cmr.ecef_} {
  }

  inline bool operator==(const camera &rhs) const noexcept {
    using namespace glm;
    return all(equal(viewport_, rhs.viewport_)) &&
           all(equal(near_far_, rhs.near_far_)) &&
           all(equal(ecef_, rhs.ecef_)) &&
           all(equal(dir_, rhs.dir_)) &&
           all(equal(up_, rhs.up_));
  }
  
  inline bool operator!=(const camera &rhs) const noexcept {

    return !(operator==(rhs));
  }

  friend std::ostream &operator<<(std::ostream &os, const camera & cmr) noexcept {
    os << "position " << glm::to_string(cmr.ecef_) << std::endl;
    os << "     dir " << glm::to_string(cmr.dir_) << std::endl;
    os << "      up " << glm::to_string(cmr.up_) << std::endl;
    os << "near far " << glm::to_string(cmr.near_far_) << std::endl;
    os << "viewport " << glm::to_string(cmr.viewport_) << std::endl;
    return os;
  }

private:
  inline double height() const noexcept {
    using namespace glm;
    dvec3 geo = ecef_;
    trans::wgs84ecef_to_geo(geo, geo);

    return geo.z;
  }

  inline void calculate_near_far() noexcept {
    using namespace glm;
    constexpr static double scalar = 1e4;
    double near, far, alt = height();
    near = ecef_.length();
    if (near > 1.1 * trans::WGS84_A) {
      far = 2 * sqrt(near * near - trans::WGS84_A * trans::WGS84_A) + scalar;
    } else {
      far = trans::WGS84_A * scalar;
    }
    near = far / scalar;
    if (near > alt / 50.0) {
      near = alt / 50.f;
      far = near * scalar;
    }

    if (far < 0.1f) {
      near = 0.1f;
      far = near * scalar;
    }

    if (near_far_.y > 0.f) {
      double curr_far = near_far_.y / 2.0;
      double sc = 1.0;
      if (far > curr_far) {
        if (near_far_.y + curr_far < far) {
          sc = (near_far_.y + curr_far) / far;
          far = near_far_.y + curr_far;
        }
      } else if (far > near_far_.y && near_far_.y - curr_far > far) {
        sc = (near_far_.y - curr_far) / far;
        far = near_far_.y - curr_far;
      }

      if (sc != 1.0) {
        near *= sc;
      }
    }

    near_far_.x = near;
    near_far_.y = far;
  }

private:
  glm::ivec2 viewport_;
  glm::dvec2 near_far_;
  glm::dvec3 up_;
  glm::dvec3 dir_;
  glm::dvec3 ecef_;
};

} // namespace esim

#endif