#ifndef __ESIM_CORE_CORE_TRANSFORM_GEO_H_
#define __ESIM_CORE_CORE_TRANSFORM_GEO_H_

#include <cstdint>
#include <functional>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/mat4x4.hpp>

namespace esim {

namespace geo {

/**
 * @brief Specifies the maptile structure.
 * 
 */
struct maptile {
  uint8_t lod;
  uint32_t x, y;
};

/**
 * @brief World Geodetic System 1984 detilas.
 * 
 */
struct wgs84 {

inline constexpr static double A         = +6.37813700000000000000e+0006;   /* a */
inline constexpr static double INVF      = +2.98257223563000000000e+0002;   /* 1/f */
inline constexpr static double F         = +3.35281066474748071998e-0003;   /* f */
inline constexpr static double INVA      = +1.56785594288739799723e-0007;   /* 1/a */
inline constexpr static double INVAA     = +2.45817225764733181057e-0014;   /* 1/(a^2) */
inline constexpr static double B         = +6.35675231424517949745e+0006;   /* b */
inline constexpr static double C         = +5.21854008423385332406e+0005;   /* c */
inline constexpr static double E         = +8.18191908426214947083e-0002;   /* e */
inline constexpr static double EE        = +6.69437999014131705734e-0003;   /* e^2 */
inline constexpr static double EED2      = +3.34718999507065852867e-0003;   /* (e^2)/2 */
inline constexpr static double EEEE      = +4.48147234524044602618e-0005;   /* e^4 */
inline constexpr static double EEEED4    = +1.12036808631011150655e-0005;   /* (e^4)/4 */
inline constexpr static double AADC      = +7.79540464078689228919e+0007;   /* (a^2)/c */
inline constexpr static double BBDCC     = +1.48379031586596594555e+0002;   /* (b^2)/(c^2) */
inline constexpr static double P1MEE     = +9.93305620009858682943e-0001;   /* 1-(e^2) */
inline constexpr static double P1MEEDAA  = +2.44171631847341700642e-0014;   /* (1-(e^2))/(a^2) */
inline constexpr static double P1MEEDB   = +1.56259921876129741211e-0007;   /* (1-(e^2))/b */
inline constexpr static double HMIN      = +2.25010182030430273673e-0014;   /* (e^12)/4 */
inline constexpr static double INVCBRT2  = +7.93700525984099737380e-0001;   /* 1/(2^(1/3)) */
inline constexpr static double INV3      = +3.33333333333333333333e-0001;   /* 1/3 */
inline constexpr static double INV6      = +1.66666666666666666667e-0001;   /* 1/6 */

};

/**
 * @brief Convert maptile to geodetic coordinate
 * 
 * @param mt specifies the target maptiles in x, y and lod.
 * @param out specifies the output vector.
 * @return the reference to result vector.
 */
template <typename type = double>
glm::vec<3, type> &maptile_to_geo(const glm::vec<3, type> &mt,
                                  glm::vec<3, type> &out) noexcept;

/**
 * @brief Trnasform position from geodetic to ECEF
 * 
 * @param geo specifies the target LLA position.
 * @param ecef specifies the ECEF position transformed from geo.
 * @return the reference to result ECEF vector.
 */
template <typename type = double, typename geosys = wgs84>
glm::vec<3, type> &geo_to_ecef(const glm::vec<3, type> &geo,
                               glm::vec<3, type> &ecef) noexcept;

/**
 * @brief Trnasform position from geodetic to ECEF
 * 
 * @param ecef specifies the target ECEF position.
 * @param geo specifies the LLA position transformed from ecef.
 * @return the reference to result LLA vector.
 */
template <typename type = double, typename geosys = wgs84>
glm::vec<3, type> &ecef_to_geo(const glm::vec<3, type> &ecef,
                               glm::vec<3, type> &geo) noexcept;

} // namespace geo

} // namespace  esim

namespace std {

template<>
struct hash<esim::geo::maptile> {
public:
  inline std::size_t operator()(const esim::geo::maptile &tile) const noexcept {
    std::size_t hash = (tile.x << 31) + tile.y;

    return hash;
  }
};

inline bool operator==(const esim::geo::maptile &lhs, const esim::geo::maptile &rhs) noexcept {
  return lhs.lod == rhs.lod &&
         lhs.x == rhs.x &&
         lhs.y == rhs.y;
}

} // namespace std

#include "geo.inl"

#endif