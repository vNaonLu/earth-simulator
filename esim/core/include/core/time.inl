namespace esim {

namespace core {

namespace details {

inline constexpr time_type count_leapyears(time_type year) noexcept {
  assert(year > 0);
  return year / 4 - year / 100 + year / 400;
}

inline time_type timeutc(tm utc) noexcept {
  assert(utc.tm_year >= 70);
  constexpr unsigned int const leaps_before_epoch = count_leapyears(1970);
  unsigned int leapdays = count_leapyears(utc.tm_year + 1899) - leaps_before_epoch;
  unsigned int unix_time;

  unix_time = ((utc.tm_year - 70) * 365 + leapdays) * 86400;
  unix_time += utc.tm_yday * 86400 + utc.tm_hour * 3600 + utc.tm_min * 60 + utc.tm_sec;

  return unix_time;
}

} // namespace details

inline time_type current_utctimestamp() noexcept {
  time_t tm = std::time(nullptr);

  return details::timeutc(*std::gmtime(&tm));
}

inline double unix_to_juliandate(time_type unix_sec) noexcept {

  return (unix_sec / 86400.0f) + 2440587.5 - 2451545.0;
}

} // namespace core

} // namespace esim
