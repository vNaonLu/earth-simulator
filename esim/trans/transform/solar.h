#ifndef __ESIM_TRANS_TRANSFORM_SOLAR_H_
#define __ESIM_TRANS_TRANSFORM_SOLAR_H_

#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

namespace esim {

namespace trans {

/**
 * @brief Specifies the direction of sun.
 * 
 * @param jd specifies the query julian date.
 * @param vec specifies the vector to receive query answer.
 * @return the result.
 */
glm::dvec3 vector_to_sun(double jd, glm::dvec3 &vec) noexcept;

/**
 * @brief Specifies the direction of sun.
 * 
 * @param jd specifies the query julian date.
 * @return the result.
 */
glm::dvec3 vector_to_sun(double jd) noexcept;

} // namespace trans

} // namespace esim

#include "solar.inl"

#endif