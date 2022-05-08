namespace esim {

inline glm::mat4x4 sun::rotate_to_solar_direction(const glm::mat4x4 &mat) const noexcept {
  using namespace glm;
  constexpr static vec3 x_axis{1.0f, 0.0f, 0.0f};
  auto RoA = cross(x_axis, direction());
  auto rad = acos(dot(x_axis, direction()));

  return rotate(mat, rad, RoA);
}

inline glm::mat4x4 sun::rotate_to_solar_direction_on_equator(const glm::mat4x4 &mat) const noexcept {
  using namespace glm;
  constexpr static vec3 x_axis{1.0f, 0.0f, 0.0f};
  constexpr static vec3 RoA{0.0f, 0.0f, 1.0f};
  auto rad = acos(dot(x_axis, direction()));

  return rotate(mat, rad, RoA);
}

inline double sun::julian_date() const noexcept {

  return jd_;
}

inline glm::vec3 sun::direction() const noexcept {
  using namespace glm;

  return static_cast<vec3>(normalize(solar_direction_));
}

inline glm::vec3 sun::direction_AU() const noexcept {
  using namespace glm;

  return solar_direction_;
}

inline float sun::visual_pixel_size_of_sun(const camera &cmr) noexcept {
  using namespace glm;
  constexpr static double AU = trans::astronomical_unit();
  dmat4x4 proj = cmr.projection<double>();
  dmat4x4 view = lookAt(dvec3(0.0f), dvec3{0.0f, 0.0f, 1.0}, dvec3{0.0, 1.0, 0.0});
  dvec4 orgin_ndc = proj * view * dvec4(0.0f, 0.0f, AU, 1.0f);
  dvec4 top_ndc = proj * view * dvec4(0.0f, radius(), AU, 1.0f);

  return max(0.001f, static_cast<float>(abs(top_ndc.y - orgin_ndc.y) / orgin_ndc.w));
}

inline constexpr double sun::radius() noexcept {

  return 6.957e8;
}

inline sun::sun() noexcept
    : jd_{trans::time::unix_to_juliandate(trans::time::current_utctimestamp())},
      solar_direction_{trans::vector_to_sun(jd_)} {}

inline sun::sun(double jd) noexcept
    : jd_{jd},
      solar_direction_{trans::vector_to_sun(jd_)} {}

} // namespace esim
