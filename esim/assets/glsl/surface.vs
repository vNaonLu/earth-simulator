#version 330
attribute vec3 attb_pos;
uniform   mat4 unfm_model;
uniform   mat4 unfm_view;
uniform   mat4 unfm_proj;
uniform   vec3 unfm_offset;

void main () {
  mat4 mvp = unfm_proj * unfm_view * unfm_model;
  gl_Position = mvp * vec4(attb_pos, 1.0);
}