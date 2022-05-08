namespace esim {

namespace geo {

template <typename type>
inline glm::vec<3, type> &maptile_to_geo(const glm::vec<3, type> &mt,
                                         glm::vec<3, type> &out) noexcept {
  using namespace glm;
  type LOD = static_cast<type>(1 << static_cast<int>(mt.z));
  type n = pi<type>() - 2.0 * pi<type>() * mt.x / LOD;
  out.y = mt.y / LOD * 360.0 - 180.0;
  out.x = 180.0 / pi<type>() * atan(0.5 * (exp(n) - exp(-n)));

  return out;
}

template <typename type, typename geosys>
inline glm::vec<3, type> &geo_to_ecef(const glm::vec<3, type> &geo,
                                      glm::vec<3, type> &ecef) noexcept {
  using namespace glm;
  const static double aadc     = geosys::AADC;      // (a^2)/c
  const static double bbdcc    = geosys::BBDCC;     // (b^2)/(c^2)
  const static double p1mee    = geosys::P1MEE;     // 1-(e^2)
  double lat, lon, alt;
  double N, d;
  double coslon, sinlon;
  double coslat, sinlat;
  lat = geo.x; lon = geo.y; alt = geo.z;
  coslat = cos(lat); sinlat = sin(lat);
  coslon = cos(lon); sinlon = sin(lon);
  N = aadc / sqrt(coslat * coslat + bbdcc);
  d = (N + alt) * coslat;

  ecef.x = static_cast<type>(d * coslon);
  ecef.y = static_cast<type>(d * sinlon);
  ecef.z = static_cast<type>((p1mee * N + alt) * sinlat);

  return ecef;
}

template <typename type, typename geosys>
inline glm::vec<3, type> &ecef_to_geo(const glm::vec<3, type> &ecef,
                                      glm::vec<3, type> &geo) noexcept {
  const static double invaa    = geosys::INVAA;     // 1/(a^2)
  const static double aadc     = geosys::AADC;      // (a^2)/c
  const static double bbdcc    = geosys::BBDCC;     // (b^2)/(c^2)
  const static double l        = geosys::EED2;      // (e^2)/2
  const static double p1mee    = geosys::P1MEE;     // 1-(e^2)
  const static double p1meedaa = geosys::P1MEEDAA;  // (1-(e^2))/(a^2)
  const static double Hmin     = geosys::HMIN;      // (e^12)/4
  const static double ll4      = geosys::EEEE;      // 4*(l^2) = e^4
  const static double ll       = geosys::EEEED4;    // l^2 = (e^4)/4
  const static double invcbrt2 = geosys::INVCBRT2;  // 1/(2^(1/3))
  const static double inv3     = geosys::INV3;      // 1/3
  const static double inv6     = geosys::INV6;      // 1/6

  double x, y, z;
  double lat, lon, alt;
  // The variables below correspond to symbols used in the paper
  // "Accurate Conversion of Earth-Centered, Earth-Fixed Coordinates
  // to Geodetic Coordinates"
  double beta, C, dFdt, dt, dw, dz, F, G, H, i, k, m, n, p, P, t, u, v, w;
  // Intermediate variables
  double j, ww, mpn, g, tt, ttt, tttt, zu, wv, invuv, da;
  double t1, t2, t3, t4, t5, t6, t7;
  x = ecef.x;
  y = ecef.y;
  z = ecef.z;
  ww = x * x + y * y;
  m = ww * invaa;
  n = z * z * p1meedaa;
  mpn = m + n;
  p = inv6 * (mpn - ll4);
  G = m * n * ll;
  H = 2 * p * p * p + G;
  if (H < Hmin) {
    assert(false);
  }
  C = pow(H + G + 2 * sqrt(H * G), inv3) * invcbrt2;
  i = -ll - 0.5 * mpn;
  P = p * p;
  beta = inv3 * i - C - P / C;
  k = ll * (ll - mpn);
  // Compute left part of t
  t1 = beta * beta - k;
  t2 = sqrt(t1);
  t3 = t2 - 0.5 * (beta + i);
  t4 = sqrt(t3);
  // Compute right part of t
  t5 = 0.5 * (beta - i);
  // t5 may accidentally drop just below zero due to numeric turbulence
  // This only occurs at latitudes close to +- 45.3 degrees
  t5 = fabs(t5);
  t6 = sqrt(t5);
  t7 = (m < n) ? t6 : -t6;
  // Add left and right parts
  t = t4 + t7;
  // Use Newton-Raphson's method to compute t correction
  j = l * (m - n);
  g = 2 * j;
  tt = t * t;
  ttt = tt * t;
  tttt = tt * tt;
  F = tttt + 2 * i * tt + g * t + k;
  dFdt = 4 * ttt + 4 * i * t + g;
  dt = -F / dFdt;
  // compute latitude (range -PI/2..PI/2)
  u = t + dt + l;
  v = t + dt - l;
  w = sqrt(ww);
  zu = z * u;
  wv = w * v;
  lat = atan2(zu, wv);
  // compute altitude
  invuv = 1 / (u * v);
  dw = w - wv * invuv;
  dz = z - zu * p1mee * invuv;
  da = sqrt(dw * dw + dz * dz);
  alt = (u < 1) ? -da : da;
  // compute longitude (range -PI..PI)
  lon = atan2(y, x);

  geo.x = lat;
  geo.y = lon;
  geo.z = alt;

  return geo;
}

} // namespace geo

} // namespace esim
