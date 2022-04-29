#ifndef __ESIM_UTILS_H_
#define __ESIM_UTILS_H_

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
inline r_ptr<type> make_ptr_r(arg_types&&... args) noexcept {

  return new (std::nothrow) type(std::forward<arg_types>(args)...);
}

template <typename type, typename... arg_types>
inline u_ptr<type> make_ptr_u(arg_types&&... args) noexcept {

  return std::make_unique<type>(std::forward<arg_types>(args)...);
}

template <typename type, typename... arg_types>
inline s_ptr<type> make_ptr_s(arg_types&&... args) noexcept {

  return std::make_shared<type>(std::forward<arg_types>(args)...);
}

} // namespace esim

#endif