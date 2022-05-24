#version 460
precision highp float;

uniform mat4 u_Modl;
uniform mat4 u_View;
uniform mat4 u_Proj;

in vec3 a_Pos;

void main (void) {
  mat4 mvp = u_Proj * u_View * u_Modl;
  gl_Position = mvp * vec4(a_Pos, 1.0);
}
