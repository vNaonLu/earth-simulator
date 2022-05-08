#version 460
precision highp float;

const float Brightness = 1.0f;
const float Contrast = 1.5f;
const vec4  AverageLuminance = vec4(vec3(2.0f), 1.0f);

uniform sampler2D u_BaseMap;

in vec2 v_TexCoord;

void main() {
  vec4 space_color;

  space_color = vec4(texture2D(u_BaseMap, v_TexCoord).rgb, 1.0f);

  /*
   * reference: https://stackoverflow.com/questions/1506299/applying-brightness-and-contrast-with-opengl-es
   * Brightness and contrast have a base value of 1.0,
   * so passing that in for both will leave the texture unchanged.
   * A value less than 1.0 will decrease brightness, and a value greater than 1.0 will increase it.
   * A value less than 1.0 for contrast will make the texture more grey (less contrast),
   * and a value greater than 1.0 will make it less grey (more contrast).

   * You can pass in (0.5, 0.5, 0.5, 1.0) for AverageLuminance for a quick-and-dirty grey value,
   * but better results will be obtained by calculating a proper average luminance for your texture.
   */
  gl_FragColor = mix(space_color * Brightness,  mix(AverageLuminance, space_color, Contrast), 0.5);
  gl_FragColor.a = 1.0;
}