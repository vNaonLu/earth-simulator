#include "test_helper.h"
#include "coord/transform.h"

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
  double latitude = esim_test::random<double>(rng, -85.f, 85.f),
         longitude = esim_test::random<double>(rng, -180.f, 180.f),
         height = esim_test::random<double>(rng, 0, 10);

  double expect_lat = latitude,
         expect_lon = longitude;

  esim::coord::geodetic_to_ecef<esim::coord::TEST>(latitude, longitude, height);
  esim::coord::ecef_to_geodetic<esim::coord::TEST>(latitude, longitude, height);
  latitude = glm::degrees(latitude);
  longitude = glm::degrees(longitude);

  EXPECT_DOUBLE_EQ(expect_lat, latitude);
  EXPECT_DOUBLE_EQ(expect_lon, longitude);
}

INSTANTIATE_TEST_SUITE_P(esim, TEST_NAME, testing::Values(0, 0, 0, 0));
