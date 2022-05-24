#version 460
precision highp float;

in vec2 a_Pos;

out vec2 v_ScreenCoord;

void main () {
  v_ScreenCoord = a_Pos;
  gl_Position = vec4(a_Pos.xy, 1.0, 1.0);
}