#include "core/transform.h"
#include "test_helper.h"

#define TEST_NAME esim_coordinate_test

class TEST_NAME : public testing::TestWithParam<size_t> {
public:
  std::mt19937_64 get_testcase(size_t seed = 0) {
    if (seed == 0) {

      return esim_test::gen_testcase();
    } else {
      
      return esim_test::gen_testcase(seed);
    }
  }

};

TEST_P(TEST_NAME, ecef_long_lat_repeatly) {
  auto rng = get_testcase(GetParam());
  glm::dvec3 pos{
      esim_test::random<double>(rng, -85.f, 85.f),
      esim_test::random<double>(rng, -180.f, 180.f),
      esim_test::random<double>(rng, 0, esim::astron::earth_major())},
      expect{pos};

    pos.x = glm::radians(pos.x);
    pos.y = glm::radians(pos.y);
    esim::geo::geo_to_ecef(pos, pos);
    esim::geo::ecef_to_geo(pos, pos);
    pos.x = glm::degrees(pos.x);
    pos.y = glm::degrees(pos.y);

    EXPECT_DOUBLE_EQ(pos.x, expect.x);
    EXPECT_DOUBLE_EQ(pos.y, expect.y);
    EXPECT_NEAR(pos.z, expect.z, 0.01);
}

INSTANTIATE_TEST_SUITE_P(esim, TEST_NAME, testing::Values(0, 0, 0, 0));
