#ifndef __ESIM_UTILS_SUN_H_
#define __ESIM_UTILS_SUN_H_

#include "transform/solar.h"
#include "transform/timing.h"

namespace esim {

class sun {
public:
  /**
   * @brief Obtain the rotate matrix which rotate the x-axis (vernal equinox) to
   * the current solar vector.
   *
   * @param mat specifies the reference matrix
   * @return a rotate matrix.
   */
  glm::mat4x4 rotate_to_solar_direction(const glm::mat4x4 &mat) const noexcept;

  /**
   * @brief Obtain the rotate matrix which rotate the x-axis (vernal equinox) to
   * the current solar vector without declination.
   *
   * @param mat specifies the reference matrix
   * @return a rotate matrix.
   */
  glm::mat4x4 rotate_to_solar_direction_on_equator(const glm::mat4x4 &mat) const noexcept;

  /**
   * @brief Obtain the julian date.
   * 
   * @return the julian date.
   */
  double julian_date() const noexcept;

  /**
   * @brief Obtain the solar direction (from origin to sun).
   * 
   * @return a 3-float vector. 
   */
  glm::vec3 direction() const noexcept;

  /**
   * @brief Construct a new sun object.
   * 
   */
  sun() noexcept;

  /**
   * @brief Construct a new sun object.
   * 
   * @param jd specifies a julian date.
   */
  sun(double jd) noexcept;

  /**
   * @brief Destroy the sun object.
   * 
   */
  ~sun() = default;

private:
  double jd_;
  glm::vec3 solar_direction_;

};

} // namespace esim

#include "sun.inl"

#endif