#version 460
precision highp float;

uniform   mat4 u_Modl;
uniform   mat4 u_View;
uniform   mat4 u_Proj;

in vec3 a_Pos;

void main () {
  gl_Position = u_Proj * u_View * u_Modl * vec4(a_Pos, 1.0);
  gl_Position.z = min(gl_Position.z, 1.0); /// fixes the depth in deepest
}