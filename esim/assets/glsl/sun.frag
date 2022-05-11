#version 460
precision highp float;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform vec4 u_SolarColor;

void main() {
  FragColor = vec4(min(u_SolarColor.rgb, vec3(1.0)), 1.0);
  BrightColor = vec4(u_SolarColor.rbg, 1.0);
}