#version 460

precision highp float;

uniform vec3 u_CameraPos;
uniform float u_OuterRadius;
uniform mat4 u_Modl;
uniform mat4 u_View;
uniform mat4 u_Proj;

in vec3 a_Pos;

out vec3 v_MieCol;
out vec3 v_RayleighCol;
out vec3 v_PosToCam;

void ONAS_CalcMieAndRayleighForSkyOutside(out vec3 out_mieCol, out vec3 out_rayleighCol, out vec3 out_posToCam, vec3 pos);

void ONAS_CalcMieAndRayleighForSkyInside(out vec3 out_mieCol, out vec3 out_rayleighCol, out vec3 out_posToCam, vec3 pos);

void main () {
  vec3 pos = (u_Modl * vec4(a_Pos, 1.0)).xyz + u_CameraPos;

  if(length(u_CameraPos) >= u_OuterRadius) {
    ONAS_CalcMieAndRayleighForSkyOutside(v_MieCol, v_RayleighCol, v_PosToCam, pos);
  } else {
    ONAS_CalcMieAndRayleighForSkyInside(v_MieCol, v_RayleighCol, v_PosToCam, pos);
  }

  mat4 mvp = u_Proj * u_View * u_Modl;
  gl_Position = mvp * vec4(a_Pos, 1.0);
}