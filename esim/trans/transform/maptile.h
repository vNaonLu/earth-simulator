#ifndef __ESIM_TRANS_TRANSFORM_MAPTILE_H_
#define __ESIM_TRANS_TRANSFORM_MAPTILE_H_

#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace esim {

namespace trans {

/**
 * @brief Transform maptile to geodetic coordinate
 *
 * @param tile specifies the target maptile details with
 * x: X_tile, y: Y_tile and z: LOD.
 * @param geo specifies the output geodetic vector.
 * @return true if transform operates successfully, false otherwise.
 */
inline bool maptile_to_geo(const glm::dvec3 &tile, glm::dvec3 &geo) noexcept {
  using namespace glm;
  double LOD = static_cast<double>(1 << static_cast<int>(tile.z));
	double n = pi<double>() - 2.0 * pi<double>() * tile.y / LOD;
  geo.y = tile.x / LOD * 360.0 - 180.0;
  geo.x = 180.0 / pi<double>() * atan(0.5 * (exp(n) - exp(-n)));
  return true;
}

} // namespace trans

} // namespace esim

#endif