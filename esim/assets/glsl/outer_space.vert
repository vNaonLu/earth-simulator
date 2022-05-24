#version 460
precision highp float;

uniform mat4 u_Modl;
uniform mat4 u_View;
uniform mat4 u_Proj;

in vec3 a_Pos;
in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main (void) {
  v_TexCoord = a_TexCoord;
  gl_Position = u_Proj * u_View * u_Modl * vec4(a_Pos, 1.0);
}
