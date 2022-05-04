#ifndef __ESIM_TRANS_TRANSFORM_SOLAR_H_
#define __ESIM_TRANS_TRANSFORM_SOLAR_H_

#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

namespace esim {

namespace trans {

inline glm::dvec3 vector_to_solar(double julian_date, glm::dvec3 &vec) noexcept {
  using namespace glm;
  double n, L, g, rg, lambda, beta, R;
  double ep, rl, re;
  n = julian_date - 2451545.0;
  L = 280.460 + 0.9856474 * n;
  g = 357.528 + 0.9856003 * n;
  rg = radians(g);
  lambda = L + 1.915 * sin(rg) + 0.02 * sin(2 * rg);
  beta = 0;
  R = 1.00014 - 0.01671 * cos(rg) - 0.00014 * cos(2 * rg);
  ep = 23.439 - 0.0000004 * n;
  rl = radians(lambda);
  re = radians(ep);

  /// Rectangular equatorial coordinaties
  vec.x = R * cos(rl);           /// march equinox
  vec.y = R * cos(re) * sin(rl);
  vec.z = R * sin(re) * sin(rl);
  return vec;
  
}

} // namespace trans

} // namespace esim

#endif