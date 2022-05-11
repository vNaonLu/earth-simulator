#ifndef __ESIM_CORE_CORE_TRANSFORM_ASTRON_H_
#define __ESIM_CORE_CORE_TRANSFORM_ASTRON_H_

#include "geo.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/mat4x4.hpp>

namespace esim {

namespace astron {

/**
 * @brief Obtain the astronomical unit in meters.
 * 
 * @return AU in meters.
 */
template <typename type = double>
constexpr type au() noexcept;

/**
 * @brief Obtain the ERA (earth rotation angle) origin by equinox.
 * 
 * @param jd specifies the julian date (J2000).
 * @return the sidereal hours in radian.
 */
template <typename type = double>
type era(double jd) noexcept;

/**
 * @brief Obtain the earth major radius.
 * 
 * @return the earth major radius in meters.
 */
template <typename type = double>
constexpr type earth_major() noexcept;

/**
 * @brief Obtain the earth minor radius.
 * 
 * @return the earth minor radius in meters.
 */
template <typename type = double>
constexpr type earth_minor() noexcept;

/**
 * @brief Obtain the solar radius.
 * 
 * @return the solar radius in meters.
 */
template <typename type = double>
constexpr type solar_radius() noexcept;

/**
 * @brief Obtain the vertor from origin to sun in rectangular
 * equator coordinates without constructing.
 *
 * @param jd specifies the julian date (J2000).
 * @param out specifies the vector where stored in.
 * @return a reference vector to sun.
 */
template <typename type = double>
glm::vec<3, type> &vec_solar_direction(double jd, glm::vec<3, type> &out) noexcept;

/**
 * @brief Obtain the vertor from origin to sun in rectangular
 * equator coordinates.
 *
 * @param jd specifies the julian date (J2000).
 * @return a vector to sun.
 */
template <typename type = double>
glm::vec<3, type> vec_solar_direction(double jd) noexcept;

/**
 * @brief Multiple the matrix with rotate equator coordinates to
 * galactic coordinates.
 * 
 * @param mat specifies the target matrix.
 * @return a rotation matrix.
 */
template <typename type = double>
glm::mat<4, 4, type> mat_equator_to_galactic(const glm::mat<4, 4, type> &mat) noexcept;

/**
 * @brief Obtain the matrix that rotate equator coordinates to
 * galactic coordinates.
 * 
 * @return a rotation matrix.
 */
template <typename type = double>
glm::mat<4, 4, type> mat_equator_to_galactic() noexcept;

} // namespace astron

} // namespace esim

#include "astron.inl"

#endif