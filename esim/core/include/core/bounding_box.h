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
   * @param initial_val specifies the initialized value.
   * @param b1 specifies the basis to box coordinate.
   * @param b2 specifies the basis to box coordinate.
   */
  bounding_box(glm::dvec3 initial_val,
               glm::dvec3 b1 = glm::dvec3{1.0f, 0.0f, 0.0f},
               glm::dvec3 b2 = glm::dvec3{0.0f, 1.0f, 0.0f}) noexcept;

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
  // auto T = dmat3x3{1.0f};

  box_[0] = T * dvec3{max_.x, min_.y, max_.z};
  box_[1] = T * dvec3{max_.x, max_.y, max_.z};
  box_[2] = T * dvec3{min_.x, min_.y, max_.z};
  box_[3] = T * dvec3{min_.x, max_.y, max_.z};

  box_[4] = T * dvec3{max_.x, min_.y, min_.z};
  box_[5] = T * dvec3{max_.x, max_.y, min_.z};
  box_[6] = T * dvec3{min_.x, min_.y, min_.z};
  box_[7] = T * dvec3{min_.x, max_.y, min_.z};
}

inline void bounding_box::update(const glm::dvec3 &pos) noexcept {
  using namespace glm;
  auto p_in_coord = coord_trans_ * pos;
  max_.x = max(max_.x,  p_in_coord.x);
  max_.y = max(max_.y,  p_in_coord.y);
  max_.z = max(max_.z,  p_in_coord.z);
  min_.x = min(min_.x,  p_in_coord.x);
  min_.y = min(min_.y,  p_in_coord.y);
  min_.z = min(min_.z,  p_in_coord.z);
}

inline bounding_box::bounding_box(glm::dvec3 initial_val,
                                  glm::dvec3 x, glm::dvec3 y) noexcept
    : coord_trans_{glm::inverse(glm::dmat3x3{glm::normalize(x),
                                             glm::normalize(y),
                                             glm::normalize(glm::cross(glm::normalize(x),
                                                                       glm::normalize(y)))})},
      max_{initial_val}, min_{initial_val} { }

} // namespace core

} // namespace esim

#endif