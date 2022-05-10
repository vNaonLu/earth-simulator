#ifndef __ESIM_UTILS_SUN_H_
#define __ESIM_UTILS_SUN_H_

#include "camera.h"
#include "core/time.h"
#include "core/transform.h"

namespace esim {

namespace scene {

class sun {
public:
  /**
   * @brief Obtain the rotate matrix which rotate the x-axis (vernal equinox) to
   * the current solar vector.
   *
   * @param mat specifies the reference matrix
   * @return a rotate matrix.
   */
  template <typename type = double>
  glm::mat<4, 4, type> rotate_to_solar_direction(const glm::mat<4, 4, type> &mat) const noexcept;

  /**
   * @brief Obtain the rotate matrix which rotate the x-axis (vernal equinox) to
   * the current solar vector without declination.
   *
   * @param mat specifies the reference matrix
   * @return a rotate matrix.
   */
  template <typename type = double>
  glm::mat<4, 4, type> rotate_to_solar_direction_on_equator(const glm::mat<4, 4, type> &mat) const noexcept;

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
  template <typename type = double>
  glm::vec<3, type> direction() const noexcept;

  /**
   * @brief Obtain the solar direction (from origin to sun) scaled in AU.
   * 
   * @return a 3-float vector. 
   */
  template <typename type = double>
  glm::vec<3, type> direction_AU() const noexcept;
  
  /**
   * @brief Obtain the visual pixel size of sun.
   * 
   * @param cmr specifies the current camera information.
   * @return the pixel radius of visual size of the sun.
   */
  static float visual_pixel_size_of_sun(const camera &cmr) noexcept;

  /**
   * @brief Obtain the radius of sun in meters.
   * 
   * @return the constant radius in meters of sun. 
   */
  constexpr static double radius() noexcept;

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

  bool operator==(const sun &rhs) const noexcept;

  bool operator!=(const sun &rhs) const noexcept;

private:
  double jd_;
  glm::dvec3 solar_direction_;

};

} // namespace scene

} // namespace esim

#include "sun.inl"

#endif