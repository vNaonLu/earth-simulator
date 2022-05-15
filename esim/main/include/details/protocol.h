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

  left = 37,
  up = 38,
  right = 39,
  down = 40,

  num1 = 49,
  num2 = 50,
};

inline constexpr static keycode_type KEY_NONE  = enums::to_raw(keycode::none);
inline constexpr static keycode_type KEY_CTRL  = enums::to_raw(keycode::ctrl);
inline constexpr static keycode_type KEY_LEFT  = enums::to_raw(keycode::left);
inline constexpr static keycode_type KEY_UP    = enums::to_raw(keycode::up);
inline constexpr static keycode_type KEY_RIGHT = enums::to_raw(keycode::right);
inline constexpr static keycode_type KEY_DOWN  = enums::to_raw(keycode::down);
inline constexpr static keycode_type KEY_ONE  = enums::to_raw(keycode::num1);
inline constexpr static keycode_type KEY_TWO  = enums::to_raw(keycode::num2);

/**
 * @brief Esim controller event type.
 * 
 */
enum class event_type {
  zoom,
  viewport,
  key_press,
  key_release,
};

inline constexpr static event_type EVENT_ZOOM       = event_type::zoom;
inline constexpr static event_type EVENT_VIEWPORT   = event_type::viewport;
inline constexpr static event_type EVENT_KEYPRESS   = event_type::key_press;
inline constexpr static event_type EVENT_KEYRELEASE = event_type::key_release;

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

    keycode_type key;
  };
};

} // namespace protocol

} // namespace esim

#endif