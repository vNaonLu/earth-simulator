#version 460
precision highp float;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragOccluders;

uniform bool      u_UseBaseMap;
uniform sampler2D u_BaseMap;

in vec3 v_Normal;
in vec2 v_TexCoord;
in vec3 v_GroundColor;
in vec3 v_Attenuation;
in vec3 v_FragPos;

void CalcLightScale(out vec3 out_lightScale, vec3 view_dir, vec3 normal, float specular_scale);

void main() {
  vec3 base_color, light_scale;

  CalcLightScale(light_scale, normalize(-v_FragPos), v_Normal, 32.0);

  if (u_UseBaseMap) {
    base_color = texture2D(u_BaseMap, v_TexCoord).rgb;
  } else {
    base_color = vec3(v_TexCoord, 1.0);
  }

  base_color = light_scale * base_color;
  base_color = v_GroundColor + v_Attenuation * base_color;

  FragColor = vec4(base_color, 1.0);
  FragOccluders = vec4(0.0, 1.0, 0.0, 1.0);
}