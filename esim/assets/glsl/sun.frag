#version 460
precision highp float;

uniform vec4  u_SolorNDCPos;
uniform float u_PixelRadius;
uniform vec2  u_Resolution;
uniform vec4  u_SolarColor;

in vec2 v_ScreenCoord;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragOccluders;

void NDCFixToZeroOne(out vec2 out_pixel, vec2 ndc) {
  out_pixel = u_Resolution * (ndc + 1.0) / 2.0;
}

void CalcSunBrightness(out float out_value, float dist_to_sun) {
  if (dist_to_sun <= u_PixelRadius) {
    out_value = 1.0;
  } else {
    float easing = (dist_to_sun - u_PixelRadius) / (30.0 * u_PixelRadius);
    out_value = 1.0 - easing;
  }
  out_value = max(out_value, 0.0);
}

void main() {
  vec2 pixel_sun, pixel_curr;
  float dist;

  NDCFixToZeroOne(pixel_sun, u_SolorNDCPos.xy);
  NDCFixToZeroOne(pixel_curr, v_ScreenCoord);

  dist = distance(pixel_sun, pixel_curr);

  if (u_SolorNDCPos.w >= 0) {

    if (dist <= u_PixelRadius) {
      FragColor = vec4(min(u_SolarColor.rgb, vec3(1.0)), 1.0);
    } else {
      FragColor = vec4(0.0);
    }

    FragOccluders = vec4(0.0, 0.0, 0.0, 1.0);
    CalcSunBrightness(FragOccluders.r, dist);

  } else {
    FragColor     = vec4(0.0);
    FragOccluders = vec4(0.0);
  }

}