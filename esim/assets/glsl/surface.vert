#version 460
precision highp float;

uniform vec3 u_CameraPos;
uniform float u_OuterRadius;
uniform mat4 u_Modl;
uniform mat4 u_View;
uniform mat4 u_Proj;

in vec3 a_Pos;
in vec3 a_Normal;
in vec2 a_TexCoord;

out vec3 v_Normal;
out vec2 v_TexCoord;
out vec3 v_GroundColor;
out vec3 v_Attenuation;

void ONAS_CalcColorsForGroundInside(out vec3 out_groundCol, out vec3 out_attenuation, vec3 pos);

void ONAS_CalcColorsForGroundOutside(out vec3 out_groundCol, out vec3 out_attenuation, vec3 pos);

void main (void) {
  // Get the ray from the camera to the vertex and its length
  //  (which is the far point of the ray passing through the atmosphere)
  vec3 pos = (u_Modl * vec4(a_Pos, 1.0)).xyz + u_CameraPos;

  if(length(u_CameraPos) >= u_OuterRadius) {
    ONAS_CalcColorsForGroundOutside(v_GroundColor, v_Attenuation, pos);
  } else {
    ONAS_CalcColorsForGroundInside(v_GroundColor, v_Attenuation, pos);
  }

  mat4 mvp = u_Proj * u_View * u_Modl;
  v_TexCoord = a_TexCoord;
  v_Normal = (u_Modl * vec4(a_Normal, 0.0)).xyz;
  gl_Position = mvp * vec4(a_Pos, 1.0);
}
