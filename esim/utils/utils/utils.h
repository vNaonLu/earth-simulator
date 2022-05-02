#ifndef __ESIM_UTILS_UTILS_H_
#define __ESIM_UTILS_UTILS_H_

#include <memory>
#include <new>
#include <functional>

namespace esim {

typedef std::function<void(const char*)> gl_error_callback;

template <typename type>
using r_ptr = type*;

template <typename type>
using u_ptr = std::unique_ptr<type>;

template <typename type>
using s_ptr = std::shared_ptr<type>;

template <typename type, typename... arg_types>
inline static r_ptr<type> make_ptr_r(arg_types&&... args) noexcept {

  return new (std::nothrow) type(std::forward<arg_types>(args)...);
}

template <typename type, typename... arg_types>
inline static u_ptr<type> make_ptr_u(arg_types&&... args) noexcept {

  return std::make_unique<type>(std::forward<arg_types>(args)...);
}

template <typename type, typename... arg_types>
inline static s_ptr<type> make_ptr_s(arg_types&&... args) noexcept {

  return std::make_shared<type>(std::forward<arg_types>(args)...);
}

inline namespace enums {

template <typename type,
          std::enable_if_t<std::is_enum_v<type>, bool> = true>
using raw = std::underlying_type_t<type>;

template <typename type,
          std::enable_if_t<std::is_enum_v<type>, bool> = true>
inline static raw<type> to_raw(type &&enm) noexcept {

  return static_cast<raw<type>>(std::forward<type>(enm));
}

template <typename type,
          std::enable_if_t<std::is_enum_v<type>, bool> = true>
inline static raw<type> operator|(const raw<type> &enm_x, type &&enm_y) noexcept {

  return enm_x | static_cast<raw<type>>(std::forward<type>(enm_y));
}

template <typename type,
          std::enable_if_t<std::is_enum_v<type>, bool> = true>
inline static raw<type> operator|(type &&enm_x, type &&enm_y) noexcept {

  return static_cast<raw<type>>(std::forward<type>(enm_x)) |
         static_cast<raw<type>>(std::forward<type>(enm_y));
}

template <typename type,
          std::enable_if_t<std::is_enum_v<type>, bool> = true>
inline static raw<type> operator|=(raw<type> &enm_x, type &&enm_y) noexcept {

  return enm_x |= static_cast<raw<type>>(std::forward<type>(enm_y));
}

template <typename type,
          std::enable_if_t<std::is_enum_v<type>, bool> = true>
inline static raw<type> operator&(const raw<type> &enm_x, type &&enm_y) noexcept {

  return enm_x & static_cast<raw<type>>(std::forward<type>(enm_y));
}

template <typename type,
          std::enable_if_t<std::is_enum_v<type>, bool> = true>
inline static raw<type> operator&(type &&enm_x, type &&enm_y) noexcept {

  return static_cast<raw<type>>(std::forward<type>(enm_x)) &
         static_cast<raw<type>>(std::forward<type>(enm_y));
}

template <typename type,
          std::enable_if_t<std::is_enum_v<type>, bool> = true>
inline static raw<type> operator&=(raw<type> &enm_x, type &&enm_y) noexcept {

  return enm_x &= static_cast<raw<type>>(std::forward<type>(enm_y));
}

template <typename type,
          std::enable_if_t<std::is_enum_v<type>, bool> = true>
inline static raw<type> operator^(const raw<type> &enm_x, type &&enm_y) noexcept {

  return enm_x ^ static_cast<raw<type>>(std::forward<type>(enm_y));
}

template <typename type,
          std::enable_if_t<std::is_enum_v<type>, bool> = true>
inline static raw<type> operator^(type &&enm_x, type &&enm_y) noexcept {

  return static_cast<raw<type>>(std::forward<type>(enm_x)) ^
         static_cast<raw<type>>(std::forward<type>(enm_y));
}

} // namespace enums

} // namespace esim

#endif