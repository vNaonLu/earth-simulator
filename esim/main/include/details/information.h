#ifndef __ESIM_ESIM_SCENE_INFORMATION_H_
#define __ESIM_ESIM_SCENE_INFORMATION_H_

#include "camera.h"
#include "sun.h"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace esim {

namespace scene {

/**
 * @brief The information for a frame rendering.
 *
 */
struct frame_info {
  class camera camera;
  class sun    sun;
  bool         is_moving        = {false};
  bool         debug_show_box   = {false};
  bool         debug_show_scene = {false};
  bool         debug_show_light = {false};
  bool         debug_show_ndc   = {false};

  /// mutable
  glm::dvec2   cursor_pos_  = {0.0, 0.0};
  glm::dvec4   cursor_wpos_ = {0.0, 0.0, 0.0, 0.0};

  inline bool expect_redraw(const frame_info &rhs) const noexcept {

    return (camera != rhs.camera) ||
           (sun != rhs.sun) ||
           (is_moving != rhs.is_moving) ||
           (debug_show_box != rhs.debug_show_box) ||
           (debug_show_light != rhs.debug_show_light) ||
           (debug_show_ndc != rhs.debug_show_ndc) ||
           (debug_show_scene != rhs.debug_show_scene);
  }

  inline bool update_cursor(const frame_info &rhs) noexcept {
    bool updated = false;
    if (cursor_pos_ != rhs.cursor_pos_) {
      updated |= true;
      cursor_pos_ = rhs.cursor_pos_;
    }
    if (cursor_wpos_ != rhs.cursor_wpos_) {
      updated |= true;
      cursor_wpos_ = rhs.cursor_wpos_;
    }

    return updated;
  }
};

} // namespace scene

} // namespace esim

#endif