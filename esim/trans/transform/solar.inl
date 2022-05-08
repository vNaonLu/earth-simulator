namespace esim {

namespace trans {

inline glm::dvec3 vector_to_sun(double jd, glm::dvec3 &vec) noexcept {
  using namespace glm;
  double n, L, g, rg, longl, lat, R;
  double ep, rl, re;
  n = jd;
  L = 280.460 + 0.9856474 * n;
  g = 357.528 + 0.9856003 * n;
  rg = radians(g);
  longl = L + 1.915 * sin(rg) + 0.02 * sin(2 * rg);
  lat = 0;
  R = 1.00014 - 0.01671 * cos(rg) - 0.00014 * cos(2 * rg);
  ep = 23.439 - 0.0000004 * n;
  rl = radians(longl);
  re = radians(ep);
  
  /// Rectangular equatorial coordinaties
  vec.x = R * cos(rl);
  vec.y = R * cos(re) * sin(rl);
  vec.z = R * sin(re) * sin(rl);
  return vec;
}

inline glm::dvec3 vector_to_sun(double jd) noexcept {
  glm::dvec3 dummy;
  return vector_to_sun(jd, dummy);
}

inline constexpr double astronomical_unit() noexcept {

  return 149'597'870'700.0f;
}

} // namespace trans

} // namespace esim