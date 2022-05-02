#ifndef __ESIM_TRANS_TRANSAFORM_WGS84_H_
#define __ESIM_TRANS_TRANSAFORM_WGS84_H_

#include "wgs84_decl.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace esim {

namespace trans {

/**
 * @brief Trnasform position from WGS84 geodetic to ECEF
 * 
 * @param geo specifies the target LLA position.
 * @param ecef specifies the ECEF position transformed from geo.
 * @return true if transformation operates successfully, false otherwise.
 */
bool wgs84geo_to_ecef(const glm::dvec3 &geo, glm::dvec3 &ecef) noexcept;

/**
 * @brief Trnasform position from WGS84 ECEF to geodetic LLA
 * 
 * @param ecef specifies the target ECEF position.
 * @param geo specifies the LLA position transformed from ecef.
 * @return true if transformation operates successfully, false otherwise.
 */
bool wgs84ecef_to_geo(const glm::dvec3 &ecef, glm::dvec3 &geo) noexcept;

} // namespace trans

} // namespace esim

#endif