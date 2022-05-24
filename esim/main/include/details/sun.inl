namespace esim {

namespace scene {

template <typename type>
inline glm::mat<4, 4, type>
sun::rotate_to_solar_direction(const glm::mat<4, 4, type> &mat) const noexcept {
  using namespace glm;
  constexpr static vec<3, type> x_axis{1.0f, 0.0f, 0.0f};
  auto RoA = cross(x_axis, direction<type>());
  auto rad = acos(dot(x_axis, direction<type>()));

  return rotate(mat, rad, RoA);
}

template <typename type>
inline glm::mat<4, 4, type>
sun::rotate_to_solar_direction_on_equator(const glm::mat<4, 4, type> &mat) const noexcept {
  using namespace glm;
  constexpr static vec<3, type> x_axis{1.0f, 0.0f, 0.0f};
  constexpr static vec<3, type> RoA{0.0f, 0.0f, 1.0f};
  auto rad = acos(dot(x_axis, direction<type>()));

  return rotate(mat, rad, RoA);
}

inline double sun::julian_date() const noexcept {

  return jd_;
}

template <typename type>
inline glm::vec<3, type> sun::direction() const noexcept {
  using namespace glm;

  return static_cast<vec<3, type>>(normalize(solar_direction_));
}

template <typename type>
inline glm::vec<3, type> sun::direction_AU() const noexcept {
  using namespace glm;

  return static_cast<vec<3, type>>(solar_direction_);
}

inline float sun::visual_pixel_size_of_sun(const camera &cmr) noexcept {
  using namespace glm;
  constexpr static double AU = esim::astron::au<double>();
  dmat4x4 proj = cmr.project<double>();
  dmat4x4 view = lookAt(dvec3(0.0f), dvec3{0.0f, 0.0f, 1.0}, dvec3{0.0, 1.0, 0.0});
  dvec4 orgin_ndc = proj * view * dvec4(0.0f, 0.0f, AU, 1.0f);
  dvec4 top_ndc = proj * view * dvec4(0.0f, radius(), AU, 1.0f);

  return max(0.1f, static_cast<float>(abs(top_ndc.y - orgin_ndc.y) / orgin_ndc.w));
}

inline constexpr double sun::radius() noexcept {

  return 6.957e8;
}

inline sun::sun() noexcept
    : jd_{core::unix_to_juliandate(core::current_utctimestamp())},
      solar_direction_{astron::vec_solar_direction<double>(jd_)} {}

inline sun::sun(double jd) noexcept
    : jd_{jd},
      solar_direction_{astron::vec_solar_direction<double>(jd_)} {}

inline bool sun::operator==(const sun &rhs) const noexcept {
  using namespace glm;
  return all(equal(solar_direction_, rhs.solar_direction_));
}

inline bool sun::operator!=(const sun &rhs) const noexcept {
  
  return !operator==(rhs);
}

} // namespace scene

} // namespace esim
