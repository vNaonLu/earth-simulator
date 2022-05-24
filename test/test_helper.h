#ifndef __ESIM_TEST_TEST_HELPER_H_
#define __ESIM_TEST_TEST_HELPER_H_

#include <gtest/gtest.h>
#include <random>

namespace esim_test {

inline static void show_testcase_seed(size_t seed) noexcept {
  std::cout << "[          ] testcase seed: " << seed << std::endl;
}

inline static std::mt19937_64 gen_testcase() noexcept {
  static std::random_device rd;
  auto seed = rd();
  show_testcase_seed(seed);
  return std::mt19937_64(seed);
}

inline static std::mt19937_64 gen_testcase(size_t seed) noexcept {
  show_testcase_seed(seed);

  return std::mt19937_64(seed);
}

template <typename type, std::enable_if_t<std::is_integral_v<type>, bool> = true>
inline static type random(std::mt19937_64 &rng,
                          type from, type to) noexcept {
  static std::uniform_int_distribution<type> U;
  decltype(U.param()) p {from, to};
  U.param(p);

  return U(rng);
}

template <typename type, std::enable_if_t<std::is_floating_point_v<type>, bool> = true>
inline static type random(std::mt19937_64 &rng,
                          type from, type to) noexcept {
  static std::uniform_real_distribution<type> U;
  decltype(U.param()) p {from, to};
  U.param(p);

  return U(rng);
}

} // namespace esim

#endif