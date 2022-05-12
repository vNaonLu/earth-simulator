#version 460
precision highp float;

in vec2 a_Pos;

out vec2 v_TexCoords;

void main () {
  v_TexCoords = (a_Pos + 1.0) / 2.0;
  gl_Position = vec4(a_Pos, 0.5, 1.0);
}