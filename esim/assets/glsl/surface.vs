#version 330
attribute vec3 attb_pos;
attribute vec2 attb_text;
uniform   mat4 unfm_model;
uniform   mat4 unfm_view;
uniform   mat4 unfm_proj;
uniform   vec3 unfm_offset;
varying   vec2 vary_tex_coord;

void main () {
  mat4 mvp = unfm_proj * unfm_view * unfm_model;
  vary_tex_coord = attb_text;
  gl_Position = mvp * vec4(attb_pos, 1.0);
}