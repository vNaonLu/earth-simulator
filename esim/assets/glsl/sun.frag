#version 460
precision highp float;

uniform vec4 u_SolarColor;

void main() {
  gl_FragColor = u_SolarColor;
}