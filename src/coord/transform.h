#ifndef __BSIM_COORD_TRANSFORM_H_
#define __BSIM_COORD_TRANSFORM_H_

#include <glad/glad.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <limits>
#include <utility>

namespace esim {

namespace coord {

struct tile_number {
  uint32_t zoom;
  uint32_t xtile,
           ytile;
};

namespace details {
double constexpr sqrt_newton_raphson(double x, double curr, double prev) {
  return curr == prev
             ? curr
             : sqrt_newton_raphson(x, 0.5 * (curr + x / curr), curr);
}
} // namespace details

/*
 * Constexpr version of the square root
 * Return value:
 *   - For a finite and non-negative value of "x", returns an approximation for the square root of "x"
 *   - Otherwise, returns NaN
 */
double constexpr sqrt(double x) {
  return x >= 0 && x < std::numeric_limits<double>::infinity()
             ? details::sqrt_newton_raphson(x, x, 0)
             : std::numeric_limits<double>::quiet_NaN();
}

enum geodetic_system {
  TEST,
  WGS84,
};

template <geodetic_system>
struct geosystem_details {};

template <>
struct geosystem_details<TEST> {
  constexpr static double semi_maj_axis = 1;
  constexpr static double first_ecc_sqr = 0;
};

template <>
struct geosystem_details<WGS84> {
  constexpr static double semi_maj_axis = 6'378'137.0L;
  constexpr static double first_ecc_sqr = 6.694'379'990'137'799'7e-3;
};

inline double tilex_to_long(uint32_t zoom, double x) {

  return x / (double)(1 << zoom) * 360.0 - 180;
}

inline double tiley_to_lati(uint32_t zoom, double y) {
  using namespace glm;
	double n = pi<double>() - 2.0 * pi<double>() * y / (double)(1 << zoom);

	return 180.0 / pi<double>() * atan(0.5 * (exp(n) - exp(-n)));
}

template <geodetic_system system = WGS84>
inline void geodetic_to_ecef(double &x, double &y, double &z) {
  using namespace glm;
  constexpr static double a = geosystem_details<system>::semi_maj_axis,
                          e2= geosystem_details<system>::first_ecc_sqr,
                          a1 = a * e2,
                          a2 = a1 * a1,
                          a3 = a1 * e2 / 2,
                          a4 = 2.5 * a2,
                          a5 = a1 + a3,
                          a6 = 1 - e2;
  double n, lat, lon, alt;
  lat = radians(x);
  lon = radians(y);
  alt = z;
  n = a / sqrt(1 - e2 * sin(lat) * sin(lat));
  x = (n + alt) * cos(lat) * cos(lon);
  y = (n + alt) * cos(lat) * sin(lon);
  z = (n * (1 - e2) + alt) * sin(lat);
}

template <geodetic_system system = WGS84>
inline void ecef_to_geodetic(double &lat, double &lon, double &alt) {
  using namespace glm;
  constexpr static double a = geosystem_details<system>::semi_maj_axis,
                          e2 = geosystem_details<system>::first_ecc_sqr,
                          a1 = a * e2,
                          a2 = a1 * a1,
                          a3 = a1 * e2 / 2,
                          a4 = 2.5 * a2,
                          a5 = a1 + a3,
                          a6 = 1 - e2;
  double zp, w2, w, r2, r, s2, c2, s, c, ss;
  double g, rg, rf, u, v, m, f, p, x, y, z;
  x = lat;
  y = lon;
  z = alt;
  zp = abs(z);
  w2 = x * x + y * y;
  w = sqrt(w2);
  r2 = w2 + z * z;
  r = sqrt(r2);
  lon = atan2(y, x); // Lon (final)
  s2 = z * z / r2;
  c2 = w2 / r2;
  u = a2 / r;
  v = a3 - a4 / r;
  if (c2 > 0.3) {
    s = (zp / r) * (1.0 + c2 * (a1 + u + s2 * v) / r);
    lat = asin(s); // Lat
    ss = s * s;
    c = sqrt(1.0 - ss);
  } else {
    c = (w / r) * (1.0 - s2 * (a5 - u - c2 * v) / r);
    lat = acos(c); // Lat
    ss = 1.0 - c * c;
    s = sqrt(ss);
  }
  g = 1.0 - e2 * ss;
  rg = a / sqrt(g);
  rf = a6 * rg;
  u = w - rg * c;
  v = zp - rf * s;
  f = c * u + s * v;
  m = c * v - s * u;
  p = m / (rf / g + f);
  lat = lat + p;
  alt = f + m * p / 2.0;
  if (z < 0.0) {
    lat *= -1.0;
  }
}

} // namespace coord

} // namespace esim

#endif
