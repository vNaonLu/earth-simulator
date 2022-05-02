#version 330
uniform vec3 unfm_offset;
varying vec2 vary_tex_coord;
uniform sampler2D basemap;

void main() {
  // gl_FragColor = vec4(unfm_offset, 1.0);
  gl_FragColor = texture(basemap, vary_tex_coord);
  gl_FragColor.a = 1.0;
}