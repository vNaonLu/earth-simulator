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

inline sun::sun() noexcept
    : jd_{trans::time::unix_to_juliandate(trans::time::current_utctimestamp())},
      solar_direction_{trans::vector_to_sun(jd_)} {}

inline sun::sun(double jd) noexcept
    : jd_{jd},
      solar_direction_{trans::vector_to_sun(jd_)} {}

} // namespace esim
