#version 460
precision highp float;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragOccluders;

uniform sampler2D u_BaseMap;

in vec3 v_Normal;
in vec2 v_TexCoord;
in vec3 v_GroundColor;
in vec3 v_Attenuation;

void CalcLightScale(out vec3 out_lightScale, vec3 normal);

void main() {
  vec3 base_color, light_scale;
  CalcLightScale(light_scale, v_Normal);

  base_color = light_scale * texture2D(u_BaseMap, v_TexCoord).rgb;
  base_color = v_GroundColor + v_Attenuation * base_color;

  FragColor = vec4(base_color, 1.0);
  FragOccluders = vec4(0.0, 1.0, 0.0, 1.0);
}