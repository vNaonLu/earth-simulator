#version 330
uniform mat4 unfm_model;
uniform mat4 unfm_vp;
uniform vec3 unfm_offset;
attribute vec3 attb_pos;

void main () {
  mat4 mvp = unfm_vp * unfm_model;
  gl_Position = mvp * vec4(attb_pos.xy, attb_pos.z, 1.0);
}