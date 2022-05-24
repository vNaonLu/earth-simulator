#version 460
precision highp float;

uniform layout(binding = 0) sampler2D u_Scene;
uniform float u_Alpha;

in vec2 v_TexCoords;

out vec4 FragColor;

void main() {
  FragColor = vec4(texture(u_Scene, v_TexCoords).rgb, u_Alpha);
}