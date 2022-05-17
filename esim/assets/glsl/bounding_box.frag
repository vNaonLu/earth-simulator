#version 460
precision highp float;

layout (location = 0) out vec4 FragColor;

uniform vec4 u_LineColor;

void main() {
  FragColor = u_LineColor;
}