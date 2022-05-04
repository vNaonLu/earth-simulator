#ifndef __ESIM_TRANS_TRANSFORM_TIMING_H_
#define __ESIM_TRANS_TRANSFORM_TIMING_H_
#define __STDC_WANT_LIB_EXT1__ 1
#define _POSIX_C_SOURCE 200112L

#include <cassert>
#include <chrono>
#include <cstdint>
#include <ctime>

namespace esim {

namespace trans {

using time_type = uint64_t;

namespace time {

using clock = std::chrono::system_clock;

namespace details {

inline constexpr unsigned int count_leapyears(unsigned int year) {
    assert(year > 0);
    return year / 4 - year / 100 + year / 400;
}

inline time_type timeutc(tm utc) {
    assert(utc.tm_year >= 70);
    constexpr unsigned int const leaps_before_epoch = count_leapyears(1970);
    unsigned int leapdays = count_leapyears(utc.tm_year + 1899) - leaps_before_epoch;
    unsigned int unix_time;

    unix_time = ((utc.tm_year - 70) * 365 + leapdays) * 86400;
    unix_time += utc.tm_yday * 86400 + utc.tm_hour * 3600 + utc.tm_min * 60 + utc.tm_sec;

    return unix_time;
}

} // namespace details

/**
 * @brief Obtain the current unix timestamp in utc seconds.
 * 
 * @return a unix timestamp.
 */
inline static time_type current_utctimestamp() noexcept {
  time_t tm = std::time(nullptr);
  return details::timeutc(*std::gmtime(&tm));
}

inline static double unix_to_juliandate(time_type unix_sec) noexcept {

  return (unix_sec / 86400.0f) + 2440587.5;
}

} // namespace time

} // namespace trans

} // namespace esim

#endif