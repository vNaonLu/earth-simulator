#ifndef __ESIM_TRANS_TRANSAFORM_WGS84_DECL_H_
#define __ESIM_TRANS_TRANSAFORM_WGS84_DECL_H_

namespace esim {

namespace trans {

inline constexpr static double WGS84_A         = +6.37813700000000000000e+0006;   /* a */
inline constexpr static double WGS84_INVF      = +2.98257223563000000000e+0002;   /* 1/f */
inline constexpr static double WGS84_F         = +3.35281066474748071998e-0003;   /* f */
inline constexpr static double WGS84_INVA      = +1.56785594288739799723e-0007;   /* 1/a */
inline constexpr static double WGS84_INVAA     = +2.45817225764733181057e-0014;   /* 1/(a^2) */
inline constexpr static double WGS84_B         = +6.35675231424517949745e+0006;   /* b */
inline constexpr static double WGS84_C         = +5.21854008423385332406e+0005;   /* c */
inline constexpr static double WGS84_E         = +8.18191908426214947083e-0002;   /* e */
inline constexpr static double WGS84_EE        = +6.69437999014131705734e-0003;   /* e^2 */
inline constexpr static double WGS84_EED2      = +3.34718999507065852867e-0003;   /* (e^2)/2 */
inline constexpr static double WGS84_EEEE      = +4.48147234524044602618e-0005;   /* e^4 */
inline constexpr static double WGS84_EEEED4    = +1.12036808631011150655e-0005;   /* (e^4)/4 */
inline constexpr static double WGS84_AADC      = +7.79540464078689228919e+0007;   /* (a^2)/c */
inline constexpr static double WGS84_BBDCC     = +1.48379031586596594555e+0002;   /* (b^2)/(c^2) */
inline constexpr static double WGS84_P1MEE     = +9.93305620009858682943e-0001;   /* 1-(e^2) */
inline constexpr static double WGS84_P1MEEDAA  = +2.44171631847341700642e-0014;   /* (1-(e^2))/(a^2) */
inline constexpr static double WGS84_P1MEEDB   = +1.56259921876129741211e-0007;   /* (1-(e^2))/b */
inline constexpr static double WGS84_HMIN      = +2.25010182030430273673e-0014;   /* (e^12)/4 */
inline constexpr static double WGS84_INVCBRT2  = +7.93700525984099737380e-0001;   /* 1/(2^(1/3)) */
inline constexpr static double WGS84_INV3      = +3.33333333333333333333e-0001;   /* 1/3 */
inline constexpr static double WGS84_INV6      = +1.66666666666666666667e-0001;   /* 1/6 */
inline constexpr static double WGS84_D2R       = +1.74532925199432957691e-0002;   /* pi/180 */
inline constexpr static double WGS84_R2D       = +5.72957795130823208766e+0001;   /* 180/pi */

} // namespace trans

} // namespace esim

#endif