#version 330
attribute vec3 attb_pos;
uniform   mat4 unfm_model;
uniform   mat4 unfm_vp;

void main () {
  gl_Position = unfm_vp * unfm_model * vec4(attb_pos, 1.0);
}