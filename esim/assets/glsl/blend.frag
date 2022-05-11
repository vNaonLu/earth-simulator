#version 460
precision highp float;

uniform layout(binding = 0) sampler2D u_Scene;
uniform layout(binding = 1) sampler2D u_BloomBlur;
uniform float u_Exposure;
uniform float u_Gamma;

in vec2 v_TexCoords;

out vec4 FragColor;

void main() {
  vec3 hdrColor = texture(u_Scene, v_TexCoords).rgb + texture(u_BloomBlur, v_TexCoords).rgb;

  vec3 result = vec3(1.0) - exp(-hdrColor * u_Exposure);
       result = pow(result, vec3(1.0 / u_Gamma));

  FragColor = vec4(result, 1.0);
}