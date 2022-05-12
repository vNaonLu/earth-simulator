#version 460
precision highp float;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform sampler2D u_BaseMap;

in vec3 v_Normal;
in vec2 v_TexCoord;
in vec3 v_GroundColor;
in vec3 v_Attenuation;

void CalcLightScale(out vec3 out_lightScale, vec3 normal);

void CalcBrightness(out vec4 out_color, vec4 color);

void main() {
  vec3 base_color, light_scale;
  CalcLightScale(light_scale, v_Normal);

  base_color = v_GroundColor + v_Attenuation * texture2D(u_BaseMap, v_TexCoord).xyz;
  base_color = light_scale * base_color;

  FragColor.rgb = base_color;
  FragColor.a = 1.0;
  CalcBrightness(BrightColor, FragColor);
}