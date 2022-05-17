#ifndef __ESIM_CORE_BOUNDING_BOX_H_
#define __ESIM_CORE_BOUNDING_BOX_H_

#include <glm/mat4x4.hpp>
#include <array>

namespace esim {

namespace core {

/**
 * @brief Bounding box structure containes 8-point information.
 * 
 */
class bounding_box {
public:
  /**
   * @brief Obtain the 8-point information.
   *
   * @return an array of 8-point which describe a box in
   * the regular coordinates.
   */
  const std::array<glm::dvec3, 8> &data() const noexcept;

  /**
   * @brief Calculates the 8-point information according to
   * the point updated before.
   *
   */
  void calculate_box() noexcept;

  /**
   * @brief Update the max-min points.
   * 
   * @param pos specifies the target point in regular coordinates.
   */
  void update(const glm::dvec3 &pos) noexcept;

  /**
   * @brief Construct a new bounding box object.
   * 
   * @param v1 specifies the basis to box coordinate.
   * @param v2 specifies the basis to box coordinate.
   * @param v3 specifies the basis to box coordinate.
   */
  bounding_box(glm::dvec3 v1 = glm::dvec3{1.0f, 0.0f, 0.0f},
               glm::dvec3 v2 = glm::dvec3{0.0f, 1.0f, 0.0f},
               glm::dvec3 v3 = glm::dvec3{0.0f, 0.0f, 1.0f}) noexcept;

  /**
   * @brief Destroy the bounding box object.
   * 
   */
  ~bounding_box() = default;

private:
  glm::dmat3x3              coord_trans_;
  glm::dvec3                max_, min_;
  std::array<glm::dvec3, 8> box_;
};

inline const std::array<glm::dvec3, 8> &bounding_box::data() const noexcept {

  return box_;
}

inline void bounding_box::calculate_box() noexcept {
  using namespace glm;
  /// top
  /// 0        1(max)
  /// 2        3
  /// bottom
  /// 4        5
  /// 6(min)   7

  auto T = inverse(coord_trans_);
  auto reg_max = T * max_,
       reg_min = T * min_;

  box_[0] = dvec3{reg_min.x, reg_max.y, reg_max.z};
  box_[1] = dvec3{reg_max.x, reg_min.y, reg_max.z};
  box_[2] = dvec3{reg_min.x, reg_max.y, reg_max.z};
  box_[3] = dvec3{reg_max.x, reg_min.y, reg_max.z};

  box_[4] = dvec3{reg_min.x, reg_max.y, reg_min.z};
  box_[5] = dvec3{reg_max.x, reg_min.y, reg_min.z};
  box_[6] = dvec3{reg_min.x, reg_max.y, reg_min.z};
  box_[7] = dvec3{reg_max.x, reg_min.y, reg_min.z};
}

inline void bounding_box::update(const glm::dvec3 &pos) noexcept {
  using namespace glm;
  max_ = max(max_, coord_trans_ * pos);
  min_ = min(min_, coord_trans_ * pos);
}

inline bounding_box::bounding_box(glm::dvec3 x,
                                  glm::dvec3 y,
                                  glm::dvec3 z) noexcept
    : coord_trans_{glm::inverse(glm::dmat3x3{x, y, z})},
      max_{std::numeric_limits<double>::min()},
      min_{std::numeric_limits<double>::max()} {}

} // namespace core

} // namespace esim

#endif