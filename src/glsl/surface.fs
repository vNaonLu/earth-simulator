#version 330
uniform vec3 unfm_offset;

void main() {
  gl_FragColor = vec4(unfm_offset, 1.0);
}