#ifndef __ESIM_CORE_CORE_TIME_H_
#define __ESIM_CORE_CORE_TIME_H_

#include <cassert>
#include <chrono>
#include <cstdint>
#include <ctime>

namespace esim {

namespace core {

typedef uint64_t                  time_type;
typedef std::chrono::system_clock clock_type;

namespace details {

constexpr time_type count_leapyears(time_type year) noexcept;

time_type timeutc(tm utc) noexcept;

} // namespace details

/**
 * @brief Obtain the current unix timestamp in utc seconds.
 * 
 * @return a unix timestamp.
 */
inline time_type current_utctimestamp() noexcept;

/**
 * @brief Obtain the julian date (J2000.0) from unix time.
 * 
 * @param unix_sec specifies the unix timestamp.
 * @return the julian time.
 */
inline double unix_to_juliandate(time_type unix_sec) noexcept;

} // namespace core

} // namespace esim

#include "time.inl"

#endif