#ifndef __ESIM_UTILS_UTILS_NUMERIC_H_
#define __ESIM_UTILS_UTILS_NUMERIC_H_

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace esim {

namespace utils {

/**
 * @brief Compute the next highest power of 2 of 32-bit
 * 
 * @param val specifies the value.
 * @return the smallest power of 2 value that is greater than val.
 */
inline uint32_t ceil2_32(uint32_t val) noexcept {
  --val;
  val |= val >> 1;
  val |= val >> 2;
  val |= val >> 4;
  val |= val >> 8;
  val |= val >> 16;
  ++val;
  return val;
}

} // namespace utils

} // namespace esim

#endif