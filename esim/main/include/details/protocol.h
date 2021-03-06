#ifndef __ESIM_ESIM_ESIM_PROTOCOL_H_
#define __ESIM_ESIM_ESIM_PROTOCOL_H_

#include "core/utils.h"
#include <cstdint>

namespace esim {

namespace protocol {

typedef uint16_t keycode_type;

/**
 * @brief Keycode definitions.
 * 
 */
enum class keycode : keycode_type {
  none = 0,
  ctrl = 17,

  pageup = 33,
  pagedown = 34,

  left = 37,
  up = 38,
  right = 39,
  down = 40,

  num1 = 49,
  num2 = 50,
  num3 = 51,

  b = 66,
};

inline constexpr static keycode_type KEY_NONE  = enums::to_raw(keycode::none);
inline constexpr static keycode_type KEY_CTRL  = enums::to_raw(keycode::ctrl);
inline constexpr static keycode_type KEY_PAGEUP  = enums::to_raw(keycode::pageup);
inline constexpr static keycode_type KEY_PAGEDOWN  = enums::to_raw(keycode::pagedown);
inline constexpr static keycode_type KEY_LEFT  = enums::to_raw(keycode::left);
inline constexpr static keycode_type KEY_UP    = enums::to_raw(keycode::up);
inline constexpr static keycode_type KEY_RIGHT = enums::to_raw(keycode::right);
inline constexpr static keycode_type KEY_DOWN  = enums::to_raw(keycode::down);
inline constexpr static keycode_type KEY_ONE  = enums::to_raw(keycode::num1);
inline constexpr static keycode_type KEY_TWO  = enums::to_raw(keycode::num2);
inline constexpr static keycode_type KEY_THREE = enums::to_raw(keycode::num3);
inline constexpr static keycode_type KEY_B  = enums::to_raw(keycode::b);

/**
 * @brief Esim controller event type.
 * 
 */
enum class event_type {
  zoom,
  viewport,
  key_press,
  key_release,
  mouse_move,
  mouse_left_press,
  mouse_left_release,
};

inline constexpr static event_type EVENT_ZOOM             = event_type::zoom;
inline constexpr static event_type EVENT_VIEWPORT         = event_type::viewport;
inline constexpr static event_type EVENT_KEYPRESS         = event_type::key_press;
inline constexpr static event_type EVENT_KEYRELEASE       = event_type::key_release;
inline constexpr static event_type EVENT_MOUSEMOVE        = event_type::mouse_move;
inline constexpr static event_type EVENT_MOUSELEFTPRESS   = event_type::mouse_left_press;
inline constexpr static event_type EVENT_MOUSELEFTRELEASE = event_type::mouse_left_release;

/**
 * @brief Event information
 * 
 */
struct event {
  event_type type;
  union {
    double  value;

    struct {
      int x, y;
    };
    
    struct {
      double cursor_x, cursor_y;
    };

    keycode_type key;
  };
};

} // namespace protocol

} // namespace esim

#endif