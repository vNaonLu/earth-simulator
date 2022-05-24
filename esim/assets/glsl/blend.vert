#version 460
precision highp float;

in vec2 a_Pos;

out vec2 v_TexCoords;

void main () {
  v_TexCoords = a_Pos;
  gl_Position = vec4(a_Pos, 0.5, 1.0);
}