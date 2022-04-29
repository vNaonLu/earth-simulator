#version 110
uniform mat4 unfm_model;
uniform mat4 unfm_view;
uniform mat4 unfm_proj;
uniform vec3 unfm_offset;
attribute vec3 attb_pos;

void main () {
  mat4 mvp = unfm_proj * unfm_view * unfm_model;
  gl_Position = mvp * vec4(attb_pos, 1.0);
}