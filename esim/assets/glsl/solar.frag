#version 330
precision highp float;
uniform vec4 unfm_color;

void main() {
  gl_FragColor = unfm_color;
}