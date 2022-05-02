#ifndef __ESIM_TRANS_TRANSFORM_TRANSFORM_HELPER_H_
#define __ESIM_TRANS_TRANSFORM_TRANSFORM_HELPER_H_

#include <limits>

namespace esim {

namespace trans {

namespace details {
double constexpr sqrt_newton_raphson(double x, double curr, double prev) {
  return curr == prev
             ? curr
             : sqrt_newton_raphson(x, 0.5 * (curr + x / curr), curr);
}
} // namespace details

/*
 * Constexpr version of the square root
 * Return value:
 *   - For a finite and non-negative value of "x", returns an approximation for the square root of "x"
 *   - Otherwise, returns NaN
 */
double constexpr sqrt(double x) {
  return x >= 0 && x < std::numeric_limits<double>::infinity()
             ? details::sqrt_newton_raphson(x, x, 0)
             : std::numeric_limits<double>::quiet_NaN();
}

} // namespace trans

} // namespace esim

#endif