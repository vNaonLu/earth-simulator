#version 330
attribute vec3 attb_pos;
uniform   mat4 unfm_model;
uniform   mat4 unfm_view;
uniform   mat4 unfm_proj;

void main () {
  gl_Position = unfm_proj * unfm_view * unfm_model * vec4(attb_pos, 1.0);
}