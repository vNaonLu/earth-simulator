#version 330
precision highp float;
in      vec3 attb_pos;
in      vec3 attb_normal;
in      vec2 attb_text;
uniform mat4 unfm_model;
uniform mat4 unfm_view;
uniform mat4 unfm_proj;
uniform vec3 unfm_offset;
out     vec2 vary_tex_coord;
out     vec3 vary_normal;

void main () {
  mat4 mvp = unfm_proj * unfm_view * unfm_model;
  vary_tex_coord = attb_text;
  vary_normal = (mvp * vec4(attb_normal, 0.0)).xyz;
  gl_Position = mvp * vec4(attb_pos, 1.0);
}