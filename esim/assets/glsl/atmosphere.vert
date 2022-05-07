#version 330
precision highp float;

uniform mat4 unfm_model;
uniform mat4 unfm_view;
uniform mat4 unfm_proj;

in vec3 attb_pos;

/// vertex in scene
out vec4 vary_wrd;
/// camera to vertex
out vec4 vary_dir;
/// vertex in pixel
out vec4 vary_pxl;

void main () {
  vary_wrd = unfm_model * vec4(attb_pos, 1.0);
  vary_dir = unfm_view * vary_wrd;
  vary_pxl = unfm_proj * vary_dir;
  gl_Position = vary_pxl;
}