#version 460
precision highp float;

#define DECAY		  .974
#define EXPOSURE	.24
#define SAMPLES	   64
#define DENSITY	  .97
#define WEIGHT		.25

uniform layout(binding = 0) sampler2D u_Scene;
uniform layout(binding = 1) sampler2D u_OccludersSampler;
uniform layout(binding = 2) sampler2D u_DitherSampler;
uniform vec4  u_LightNDCSunPos;
uniform bool  u_EnableScattering;
uniform vec2  u_Resolution;
uniform vec2  u_DitherResolution;
uniform float u_Exposure;
uniform float u_Gamma;

in vec2 v_TexCoords;

out vec4 FragColor;

bool IsVisible(vec4 normalized_sun) {
  
  return normalized_sun.w >= 0;
}

float Noise(float t) {

  return texture(u_DitherSampler, vec2(t, .0) * u_Resolution / u_DitherResolution).r;
}

float Noise(vec2 coord) {

  return texture(u_DitherSampler, coord * u_Resolution / u_DitherResolution).r;
}

void NormalizeNDC(out vec2 out_pixel, vec2 ndc) {
  out_pixel.xy = (ndc.xy + 1.0) / 2.0;
}

float GetLightScale(vec2 uv) {
  if (uv.s > 1.0 || uv.s < 0.0 || uv.t > 1.0 || uv.t < 0.0) {

    return 1.0;
  } else {

    return texture(u_OccludersSampler, uv).r;
  }
}

vec3 CalcLensFlare(vec2 uv, vec2 pos) {
  vec2 main = uv - pos;
  vec2 uvd = uv * (length(uv));

  float ang = atan(main.x, main.y);
  float dist = length(main);
  dist = pow(dist, .1);

  float f0 = 1.0 / (length(uv - pos) * 16.0 + 1.0);

  f0 = f0 + f0 * (sin(Noise(sin(ang * 2. + pos.x) * 4.0 - cos(ang * 3. + pos.y)) * 16.) * 0.1 + dist * 0.1 + 0.8);

  float f2  = max(1.0 / (1.0 + 32.0 * pow(length(uvd + 0.8 * pos), 2.0)), 0.0) * 00.25;
  float f22 = max(1.0 / (1.0 + 32.0 * pow(length(uvd + 0.85 * pos), 2.0)), 0.0) * 00.23;
  float f23 = max(1.0 / (1.0 + 32.0 * pow(length(uvd + 0.9 * pos), 2.0)), 0.0) * 00.21;

  vec2 uvx = mix(uv, uvd, -0.5);

  float f4  = max(0.01 - pow(length(uvx + 0.4  * pos), 2.4), 0.0) * 6.0;
  float f42 = max(0.01 - pow(length(uvx + 0.45 * pos), 2.4), 0.0) * 5.0;
  float f43 = max(0.01 - pow(length(uvx + 0.5  * pos), 2.4), 0.0) * 3.0;

  uvx = mix(uv, uvd, -0.4);

  float f5  = max(0.01 - pow(length(uvx + 0.2 * pos), 5.5), 0.0) * 2.0;
  float f52 = max(0.01 - pow(length(uvx + 0.4 * pos), 5.5), 0.0) * 2.0;
  float f53 = max(0.01 - pow(length(uvx + 0.6 * pos), 5.5), 0.0) * 2.0;

  uvx = mix(uv, uvd, -0.5);

  float f6  = max(0.01 - pow(length(uvx - 0.3   * pos), 1.6), 0.0) * 6.0;
  float f62 = max(0.01 - pow(length(uvx - 0.325 * pos), 1.6), 0.0) * 3.0;
  float f63 = max(0.01 - pow(length(uvx - 0.35  * pos), 1.6), 0.0) * 5.0;

  vec3 c = vec3(0.0);

  c.r += f2  + f4  + f5  + f6 ;
  c.g += f22 + f42 + f52 + f62;
  c.b += f23 + f43 + f53 + f63;
  c  = (c * 1.3 - vec3(length(uvd) * .05)) * 0.3;
  c += vec3(f0) * 0.01;

  return c;
}

float GetSunRadialBlur(vec2 fragCoord, vec2 normalize_sun) {
  float lighting;
  vec2 coord;

  coord = fragCoord.xy;

  vec2  dtc = (coord - normalize_sun) * (1. / float(SAMPLES) * DENSITY);
  float occ = texture(u_OccludersSampler, coord).r;
  float dither = Noise(fragCoord); 
  float illumdecay = 1.;

  for(int i = 0; i < SAMPLES; i++) {
    coord -= dtc;
    lighting = texture(u_OccludersSampler, coord + (dtc * dither)).r;
    lighting *= illumdecay * 0.25;
    occ += lighting;
    illumdecay *= DECAY;
  }

  return occ;
}

vec3 CalcLightEffect(vec2 fragCoord) {
  vec2 screen_pos, sun_pos;

  NormalizeNDC(sun_pos, u_LightNDCSunPos.xy);
  NormalizeNDC(screen_pos, fragCoord);

  if (IsVisible(u_LightNDCSunPos)) {
    vec3 sun_blur = vec3(1.4, 1.2, 1.0) * GetSunRadialBlur(screen_pos, sun_pos);
    vec3 sun_flare = vec3(1.4, 1.2, 1.0) * CalcLensFlare(screen_pos - 0.5, sun_pos.xy - 0.5) * GetLightScale(sun_pos);

    return sun_flare + sun_blur;
  } else {

    return vec3(0.0);
  }
}

vec3 CalcScene(vec2 fragCoord) {
  NormalizeNDC(fragCoord, fragCoord);

  return texture(u_Scene, fragCoord).rgb;
}

void main() {
  vec3 color = CalcScene(v_TexCoords);
  color.rgb = vec3(1.0) - exp(-color.rgb * u_Exposure);
  color.rgb = pow(color.rgb, vec3(1.0 / u_Gamma));

  if (u_EnableScattering) {
    color += CalcLightEffect(v_TexCoords);
  }

  FragColor = vec4(color.rgb, 1.0);
}