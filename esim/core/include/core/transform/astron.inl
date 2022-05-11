namespace esim {

namespace astron {

template <typename type>
inline constexpr type au() noexcept {

  return static_cast<type>(149'597'870'700.0f);
}

template <typename type>
inline type era(double jd) noexcept {
  using namespace glm;

  return static_cast<type>(2.0 * pi<type>() *
                           (0.779'057'273'2640 + 1.002'737'811'911'354'48 * jd));
}

template <typename type>
inline constexpr type earth_major() noexcept {

  return static_cast<type>(geo::wgs84::A);
}

template <typename type>
inline constexpr type earth_minor() noexcept {

  return static_cast<type>(geo::wgs84::B);
}

template <typename type>
inline constexpr type solar_radius() noexcept {

  return static_cast<type>(6.957e8);
}

template <typename type>
inline glm::vec<3, type> &vec_solar_direction(double jd,
                                              glm::vec<3, type> &out) noexcept {
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
  
  out.x = static_cast<type>(R * cos(rl));
  out.y = static_cast<type>(R * cos(re) * sin(rl));
  out.z = static_cast<type>(R * sin(re) * sin(rl));

  return out;
}

template <typename type>
inline glm::vec<3, type> vec_solar_direction(double jd) noexcept {
  glm::vec<3, type> dummy;

  return vec_solar_direction(jd, dummy);
}

template <typename type>
inline glm::mat<4, 4, type> mat_equator_to_galactic(const glm::mat<4, 4, type> &mat) noexcept {
  /// from equinox to galactic center
  using namespace glm;
  /// galactic center locate at
  /// 17h 45m 40.0409s, −29° 00′ 28.118″ (J2000) By Wikipedia
  float RA = radians((17.0f * 15.0f) +
                     (45.0f * (15.0f / 60.0f)) +
                     (40.0409f * (15.0f / 60.0f / 60.0f)));
  float Dc = radians(-29.0f +
                     (0.0f / 60.0f) +
                     (28.118f / 60.0f / 60.0f));
  vec<3, type> eqinox = normalize(vec<3, type>(1.0f, 0.0f, 0.0f));
  vec<3, type> gal_center = normalize(vec<3, type>{static_cast<type>(cos(Dc) * cos(RA)),
                                                   static_cast<type>(cos(Dc) * sin(RA)),
                                                   static_cast<type>(sin(Dc))});
  vec<3, type> rotation_axis = cross(eqinox, gal_center);

  return rotate(mat, static_cast<type>(acos(dot(eqinox, gal_center))), rotation_axis);
}

template <typename type>
inline glm::mat<4, 4, type> mat_equator_to_galactic() noexcept {
  
  return mat_equator_to_galactic(glm::mat<4, 4, type>{1.0f});
}

} // namespace astron

} // namespace esim
