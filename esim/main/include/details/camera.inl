namespace esim {

namespace scene {

template <typename type>
inline glm::mat<4, 4, type> camera::project() const noexcept {
  using namespace glm;
  auto aspect = viewport_.y == 0.0f
                    ? static_cast<type>(0.0f)
                    : static_cast<type>(viewport_.x) / static_cast<type>(viewport_.y);

  return perspective(static_cast<type>(radians(45.f)), aspect,
                     static_cast<type>(near_far_.x),
                     static_cast<type>(near_far_.y));
}

template <typename type>
inline glm::mat<4, 4, type> camera::skysphere_project() const noexcept {
  using namespace glm;
  auto aspect = viewport_.y == 0.0f
                    ? static_cast<type>(0.0f)
                    : static_cast<type>(viewport_.x) / static_cast<type>(viewport_.y);

  return perspective(static_cast<type>(radians(45.f)), aspect,
                     static_cast<type>(0.0f),
                     static_cast<type>(sqrt(2.0f)));
}

template <typename type>
inline glm::mat<4, 4, type> camera::view() const noexcept {
    using namespace glm;
    constexpr static vec<3, type> pos{static_cast<type>(0.0f)};
    return lookAt(pos, dir<type>(), up<type>());
}

template <typename type>
inline glm::mat<4, 4, type> camera::translate(const glm::mat<4, 4, type> &mat,
                                              const glm::vec<3, type> offset) const noexcept {
  using namespace glm;
  return glm::translate(mat, offset - pos<type>());
}

inline glm::ivec2 camera::viewport() const noexcept {

  return viewport_;
}

template <typename type>
inline glm::vec<2, type> camera::near_far() const noexcept {
  using namespace glm;
  return vec<2, type>(static_cast<type>(near_far_.x),
                      static_cast<type>(near_far_.y));
}

template <typename type>
inline glm::vec<3, type> camera::up() const noexcept {
  using namespace glm;
  return vec<3, type>(static_cast<vec<3, type>>(up_));
}

template <typename type>
inline glm::vec<3, type> camera::dir() const noexcept{
  using namespace glm;
  return vec<3, type>(static_cast<vec<3, type>>(view_));
}

template <typename type>
inline glm::vec<3, type> camera::pos() const noexcept {
  using namespace glm;
  return vec<3, type>(static_cast<vec<3, type>>(ecef_));
}

inline void camera::set_camera(const glm::dvec3 &pos, const glm::dvec3 &view,
                               const glm::dvec3 &up) noexcept {
  using namespace glm;
  ecef_ = pos;
  view_ = normalize(view);
  up_ = normalize(up);
  if (!any(isinf(ecef_)) && !any(isinf(view_)) && !any(isinf(up_))) {
    if (double dotprod = dot(view_, up_); dotprod != 0.0f) {
      /// adjust up direction
      /// to make sure there is 90 degrees between dir and up
      up_ = normalize(up_ - view_ * dotprod);
    }
    calculate_near_far();
  } else {
    assert(false);
    reset();
  }
}

inline void camera::set_viewport(int width, int height) noexcept {
  viewport_.x = width;
  viewport_.y = height;
}

inline void camera::reset() noexcept {
  using namespace glm;
  up_ = dvec3{0.0f, 0.0f, 1.0f};
  view_ = dvec3{-1.0f, 0.0f, 0.0f};
  ecef_ = dvec3{astron::earth_major() * 3.0, 0.0f, 0.0f};
  viewport_ = ivec2{0, 0};
  calculate_near_far();
}

inline camera::camera() noexcept {
  reset();
}

inline camera::~camera() noexcept {}

inline double camera::height() const noexcept {
  using namespace glm;
  dvec3 geo = ecef_;
  geo::ecef_to_geo(geo, geo);

  return geo.z;
}

inline void camera::calculate_near_far() noexcept {
  using namespace glm;
  constexpr static double scalar = 1e4;
  double near, far, alt = height();
  near = ecef_.length();
  if (near > 1.1 * astron::earth_major()) {
    far = 2 * sqrt(near * near - astron::earth_major() * astron::earth_major()) + scalar;
  } else {
    far = astron::earth_major() * scalar;
  }
  near = far / scalar;
  if (near > alt / 50.0) {
    near = alt / 50.f;
    far = near * scalar;
  }

  if (far < 0.1f) {
    near = 0.1f;
    far = near * scalar;
  }

  if (near_far_.y > 0.f) {
    double curr_far = near_far_.y / 2.0;
    double sc = 1.0;
    if (far > curr_far) {
      if (near_far_.y + curr_far < far) {
        sc = (near_far_.y + curr_far) / far;
        far = near_far_.y + curr_far;
      }
    } else if (far > near_far_.y && near_far_.y - curr_far > far) {
      sc = (near_far_.y - curr_far) / far;
      far = near_far_.y - curr_far;
    }

    if (sc != 1.0) {
      near *= sc;
    }
  }

  near_far_.x = near;
  near_far_.y = far;
}

inline bool camera::operator!=(const camera &rhs) const noexcept {

  return !(operator==(rhs));
}

inline bool camera::operator==(const camera &rhs) const noexcept {
  using namespace glm;
  return all(equal(viewport_, rhs.viewport_)) &&
         all(equal(near_far_, rhs.near_far_)) &&
         all(equal(ecef_, rhs.ecef_)) &&
         all(equal(view_, rhs.view_)) &&
         all(equal(up_, rhs.up_));
}

inline std::ostream &operator<<(std::ostream &os, const camera &cmr) noexcept {
  os << "position " << glm::to_string(cmr.ecef_) << std::endl;
  os << "     dir " << glm::to_string(cmr.view_) << std::endl;
  os << "      up " << glm::to_string(cmr.up_) << std::endl;
  os << "near far " << glm::to_string(cmr.near_far_) << std::endl;
  os << "viewport " << glm::to_string(cmr.viewport_) << std::endl;
  return os;
}

} // namespace scene

} // namespace esim
