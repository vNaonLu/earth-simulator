#version 330
precision highp float;
uniform vec3 unfm_offset;
uniform sampler2D basemap;
in vec2 vary_tex_coord;
in vec3 vary_normal;


void main() {
  gl_FragColor = texture(basemap, vary_tex_coord);
  gl_FragColor.a = 1.0;
}