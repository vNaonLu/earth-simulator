#version 460
precision highp float;

uniform vec3 u_LightDir;
uniform sampler2D u_BaseMap;

in vec3 v_Normal;
in vec2 v_TexCoord;
in vec3 v_GroundColor;
in vec3 v_Attenuation;

void CalcLightScale(out vec3 out_lightScale, vec3 normal);

void main() {
  vec3 light_scale;
  vec3 base_color = v_GroundColor + texture2D(u_BaseMap, v_TexCoord).xyz * v_Attenuation;

  CalcLightScale(light_scale, v_Normal);

  gl_FragColor.rgb = light_scale * base_color;
  gl_FragColor.a = 1.0;
}