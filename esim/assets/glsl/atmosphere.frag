#version 330
precision highp float;

#define SURFACE_MAJOR_RADIUS 6378134.0
#define SURFACE_MINOR_RADIUS 6356752.314145
#define ATMOSPHERE_THICKNESS 100000

/// vertex in scene
in vec4 vary_wrd;
/// camera to vertex
in vec4 vary_dir;
/// vertex in pixel
in vec4 vary_pxl;

// atmosphere scattering color coefficients (r, g, b, ambient)
uniform vec4  unfm_scatter_coefficients;
// uniform vec4  unfm_color;
uniform vec3  unfm_solar_dir;
uniform float unfm_view_depth;
// const vec3 sun_color = vec3(1.0, 1.0, 1.0);

float view_depth_l0 = -1.0,
      view_depth_l1 = -1.0;
bool _view_depth_l0 = false;
bool _view_depth_l1 = false;

const vec3 earth_radius = vec3(SURFACE_MAJOR_RADIUS,
                               SURFACE_MAJOR_RADIUS,
                               SURFACE_MINOR_RADIUS);

bool _view_depth(vec3 _p0, vec3 _dp) {
  vec3 p0, dp;
  float a, b, c, d, l0, l1;
  view_depth_l0 = -1.0;
  _view_depth_l0 = false;
  view_depth_l1 = -1.0;
  _view_depth_l1 = false;

  p0 = vec3(_p0);
  dp = vec3(_dp);

  // quadratic equation a.l.l+b.l+c=0; l0,l1=?;
  a = (dp.x * dp.x * earth_radius.x) +
      (dp.y * dp.y * earth_radius.y) +
      (dp.z * dp.z * earth_radius.z);

  b = (p0.x * dp.x * earth_radius.x) +
      (p0.y * dp.y * earth_radius.y) +
      (p0.z * dp.z * earth_radius.z);
  b *= 2.0;

  c = (p0.x * p0.x * earth_radius.x) +
      (p0.y * p0.y * earth_radius.y) +
      (p0.z * p0.z * earth_radius.z) - 1.0;

  // discriminant d=sqrt(b.b-4.a.c)
  d = ((b * b) - (4.0 * a * c));
  if(d < 0.0) {

    return false;
  }
  d = sqrt(d);

  // standard solution l0,l1=(-b +/- d)/2.a
  a *= 2.0;
  l0 = (-b + d) / a;
  l1 = (-b - d) / a;

  // alternative solution q=-0.5*(b+sign(b).d) l0=q/a; l1=c/q; (should be more accurate sometimes)
  //  if (b<0.0) d=-d; d=-0.5*(b+d);
  //  l0=d/a;
  //  l1=c/d;
  // sort l0,l1 asc
  if((l0 < 0.0) || ((l1 < l0) && (l1 >= 0.0))) {
    a = l0;
    l0 = l1;
    l1 = a;
  }

  // exit
  if(l1 >= 0.0) {
    view_depth_l1 = l1;
    _view_depth_l1 = true;
  }

  if(l0 >= 0.0) {
    view_depth_l0 = l0;
    _view_depth_l0 = true;

    return true;
  }

  return false;
}

// determine if ray (p0,dp) hits a sphere ((0,0,0),r)
// where r is (sphere radius)^-2
bool _star_colide(vec3 _p0, vec3 _dp) {
  vec3 p0, dp;
  float a, b, c, d, l0, l1;

  // conversion to double
  p0 = vec3(_p0);
  dp = vec3(_dp);

  // quadratic equation a.l.l+b.l+c=0; l0,l1=?;
  a = (dp.x * dp.x * earth_radius.x) +
      (dp.y * dp.y * earth_radius.y) +
      (dp.z * dp.z * earth_radius.y);

  b = (p0.x * dp.x * earth_radius.x) +
      (p0.y * dp.y * earth_radius.y) +
      (p0.z * dp.z * earth_radius.y);
  b *= 2.0;

  c = (p0.x * p0.x * earth_radius.x) +
      (p0.y * p0.y * earth_radius.y) +
      (p0.z * p0.z * earth_radius.y) - 1.0;

  // discriminant d=sqrt(b.b-4.a.c)
  d = ((b * b) - (4.0 * a * c));
  if(d < 0.0){

    return false;
  }
  d = sqrt(d);

  // standard solution l0,l1=(-b +/- d)/2.a
  a *= 2.0;
  l0 = (-b + d) / a;
  l1 = (-b - d) / a;

  // alternative solution q=-0.5*(b+sign(b).d) l0=q/a; l1=c/q; (should be more accurate sometimes)
  //  if (b<0.0) d=-d; d=-0.5*(b+d);
  //  l0=d/a;
  //  l1=c/d;
  // sort l0,l1 asc
  if(abs(l0) > abs(l1)) {
    a = l0;
    l0 = l1;
    l1 = a;
  }
  
  if(l0 < 0.0) {
    a = l0;
    l0 = l1;
    l1 = a;
  }
  if(l0 < 0.0) {

    return false;
  } else {

    return true;

  }
}

// compute atmosphere color between ellipsoids (planet_pos,planet_r) and (planet_pos,planet_R) for ray(pixel_pos,pixel_nor)
vec4 atmosphere() {
  const int n = 8;
  const float _n = 1.0 / float(n);
  int i;
  bool b0, b1;
  vec3 p0, p1, dp, p, b;
  vec4 c;     // c - color of pixel from start to end

  float h, dl, ll;
  float l0, l1, l2;
  bool e0, e1, e2;

  // a=0.0 full background color, a=1.0 no background color (ignore star)
  c = vec4(0.0, 0.0, 0.0, 0.0);

  b1 = _view_depth(vary_wrd.xyz, vary_dir.xyz);
  if(!b1) {
    // completly outside atmosphere
    return c;
  }

  e1 = _view_depth_l0;
  l1 = view_depth_l0;        // first atmosphere hit
  e2 = _view_depth_l1;
  l2 = view_depth_l1;        // second atmosphere hit
  b0 = _view_depth(vary_wrd.xyz, vary_dir.xyz);

  e0 = _view_depth_l0;
  l0 = view_depth_l0;        // first surface hit

  if((b0) && (view_depth_l1 < 0.0)) {
    // under ground
    return c;
  }
    
  // set l0 to view depth and p0 to start point
  dp = vary_dir.xyz;
  p0 = vary_wrd.xyz;
  if(!b0) {
    // outside surface
    if(!e2) {
       // inside atmosphere to its boundary
      l0 = l1;
    } else {
       // throu atmosphere from boundary to boundary
      p0 = vec3(vec3(p0) + (vec3(dp) * l1));
      l0 = l2 - l1;
    }

    // if a light source is in visible path then start color is light source color
    // if(_star_colide(p0.xyz - light_posr[i].xyz, dp.xyz, light_posr[i].a * 0.75)) {
    //   // 0.75 is enlargment to hide star texture corona
    //   c.rgb += sun_color;
    //   // ignore already drawed local star color
    //   c.a = 1.0;
    // }

  } else {
    // into surface
    if(l1 < l0) {
      // from atmosphere boundary to surface
      p0 = vec3(p0 + dp * l1);
      l0 = l0 - l1;
    } else {
      // inside atmosphere to surface
      l0 = l0;
    }
  }
  // set p1 to end of view depth, dp to intergral step
  p1 = vec3(p0 + dp * l0);
  dp = p1 - p0;
  dp *= _n;

  dl = float(l0) * _n / unfm_view_depth;
  ll = unfm_scatter_coefficients.a;
  ll += dot(normalize(p1), -unfm_solar_dir);

  for(p = p1, i = 0; i < n; p -= dp, i++) {
    b = normalize(p) * earth_radius;
    h = length(p - b);
    h = exp(h / ATMOSPHERE_THICKNESS) / 2.78;

    b = unfm_scatter_coefficients.rgb * h * dl;
    c.r *= 1.0 - b.r;
    c.g *= 1.0 - b.g;
    c.b *= 1.0 - b.b;
    c.rgb += b * ll;
  }

  c.r = max(c.r, 0.0);
  c.g = max(c.g, 0.0);
  c.b = max(c.b, 0.0);
  h = 0.0;

  h = max(h, c.r);
  h = max(h, c.g);
  h = max(h, c.b);

  if(h > 1.0) {
    h = 1.0 / h;
    c.r *= h;
    c.g *= h;
    c.b *= h;
  }
  
  return c;
}

void main() {
  gl_FragColor = atmosphere();
  gl_FragColor = vec4(0.2, 0.2, 0.8, 1.);
}