#ifndef __ESIM_CORE_CORE_UTILS_H_
#define __ESIM_CORE_CORE_UTILS_H_

#include <functional>
#include <memory>
#include <new>

namespace esim {

template <typename type>
using rptr = type*;

template <typename type>
using uptr = std::unique_ptr<type>;

template <typename type>
using sptr = std::shared_ptr<type>;

template <typename type, typename... arg_types>
inline static rptr<type> make_rptr(arg_types&&... args) noexcept {

  return new (std::nothrow) type(std::forward<arg_types>(args)...);
}

template <typename type, typename... arg_types>
inline static uptr<type> make_uptr(arg_types&&... args) noexcept {

  return std::make_unique<type>(std::forward<arg_types>(args)...);
}

template <typename type, typename... arg_types>
inline static sptr<type> make_sptr(arg_types&&... args) noexcept {

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