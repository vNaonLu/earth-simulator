#ifndef __ESIM_UTILS_UTILS_KEYS_H_
#define __ESIM_UTILS_UTILS_KEYS_H_

#include <glfw/glfw3.h>

namespace esim {

namespace utils {

inline constexpr static int ctrl_key = 17;

inline constexpr static int left_key = 37;
inline constexpr static int up_key = 38;
inline constexpr static int right_key = 39;
inline constexpr static int down_key = 40;

inline int glfw_to_keycode(int glfw_key) noexcept {
  switch(glfw_key) {
  case GLFW_KEY_LEFT_CONTROL:
    [[fallthrough]];
  case GLFW_KEY_RIGHT_CONTROL:
    return 17;

  case GLFW_KEY_LEFT:
    return left_key;
  case GLFW_KEY_UP:
    return up_key;
  case GLFW_KEY_RIGHT:
    return right_key;
  case GLFW_KEY_DOWN:
    return down_key;

  default:
    return -1;
  }
}

} // namespace utils

} // namespace esim

#endif