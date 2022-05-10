#ifndef __ESIM_ESIM_SCENE_CAMERA_H_
#define __ESIM_ESIM_SCENE_CAMERA_H_

#include "core/transform.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

namespace esim {

namespace scene {

/**
 * @brief The camera class in the scene.
 * 
 */
class camera {
public:
  /**
   * @brief Obtain the projection matrix.
   * 
   * @param mat specifies the matrix which multiply.
   * @return the projection matrix.
   */
  template <typename type = double>
  glm::mat<4, 4, type> project() const noexcept;

  /**
   * @brief Obtain the projection matrix for skysphere.
   * 
   * @param mat specifies the matrix which multiply.
   * @return the projection matrix.
   */
  template <typename type = double>
  glm::mat<4, 4, type> skysphere_project() const noexcept;

  /**
   * @brief Obtain the view matrix.
   * 
   * @param mat specifies the matrix which multiply.
   * @return the view matrix from camera.
   */
  template <typename type = double>
  glm::mat<4, 4, type> view() const noexcept;

  /**
   * @brief Obtain the translate matrix.
   * 
   * @param mat specifies the matrix which multiply.
   * @param offset specifies the target position.
   * @return the matrix translate from camera.
   */
  template <typename type = double>
  glm::mat<4, 4, type> translate(const glm::mat<4, 4, type> &mat,
                              const glm::vec<3, type> offset) const noexcept;

  /**
   * @brief Obtain the width and height of the viewport.
   * 
   * @return vector with width and height.
   */
  glm::ivec2 viewport() const noexcept;

  /**
   * @brief Obtain the near and far of the camera.
   * 
   * @return vector with far and near.
   */
  template <typename type = double>
  glm::vec<2, type> near_far() const noexcept;

  /**
   * @brief Obtain the head-up direction of the camera.
   * 
   * @return the head-up direction of camera.
   */
  template <typename type = double>
  glm::vec<3, type> up() const noexcept;

  /**
   * @brief Obtain the view direction of the camera.
   * 
   * @return the view direction of camera.
   */
  template <typename type = double>
  glm::vec<3, type> dir() const noexcept;

  /**
   * @brief Obtain the ECEF position of the camera.
   * 
   * @return the ECEF position of camera.
   */
  template <typename type = double>
  glm::vec<3, type> pos() const noexcept;

  /**
   * @brief Set the information to the camera
   * 
   * @param pos specifies the ECEF position of camera.
   * @param view specifies the view direction of camera.
   * @param up specifies the head-up direction of camera.
   */
  void set_camera(const glm::dvec3 &pos,
                  const glm::dvec3 &view,
                  const glm::dvec3 &up) noexcept;

  /**
   * @brief Set the viewport to the camera.
   * 
   * @param width specifies the width of viewport.
   * @param height specifies the height of viewport.
   */
  void set_viewport(int width, int height) noexcept;

  /**
   * @brief Reset the information about camera.
   * 
   */
  void reset() noexcept;

  /**
   * @brief Construct a new camera object.
   * 
   */
  camera() noexcept;

  /**
   * @brief Destroy the camera object.
   * 
   */
  ~camera() noexcept;

  bool operator==(const camera &rhs) const noexcept;

  bool operator!=(const camera &rhs) const noexcept;

  friend std::ostream &operator<<(std::ostream &os, const camera &cmr) noexcept;

private:
  double height() const noexcept;

  void calculate_near_far() noexcept;

private:
  glm::ivec2 viewport_;
  glm::dvec2 near_far_;
  glm::dvec3 ecef_, view_, up_;
};

} // namespace scene

} // namespace esim

#include "camera.inl"

#endif