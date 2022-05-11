#version 460
precision highp float;

uniform float u_NDCSize;
uniform vec2 u_Viewport;
uniform mat4 u_Modl;
uniform mat4 u_View;
uniform mat4 u_Proj;

in vec3 a_Pos;

void main () {
  vec4 screen_pos;
  screen_pos    = u_Proj * u_View * u_Modl * vec4(a_Pos.x, 0.0, 0.0, 1.0);
  screen_pos.xy = ((screen_pos.xy / screen_pos.w) + u_NDCSize * a_Pos.yz / u_Viewport.xy) * screen_pos.w;
  /// fix the depth to deepest
  screen_pos.z = 1.0 * screen_pos.w;

  gl_Position = screen_pos;
}