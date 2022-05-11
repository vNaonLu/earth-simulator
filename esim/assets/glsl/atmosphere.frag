#version 460
precision highp float;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform vec3 u_LightDir;
uniform float u_g;

in vec3 v_MieCol;
in vec3 v_RayleighCol;
in vec3 v_PosToCam;

float ONAS_CalcMiePhase(float cosA, float g);

void CalcBrightness(out vec4 out_color, vec4 color);

void main() {

  float cosA = dot(-u_LightDir, v_PosToCam) / length(v_PosToCam);

  float miePhase = ONAS_CalcMiePhase(cosA, u_g);

  vec3 outCol = v_RayleighCol + miePhase * v_MieCol;

  FragColor = vec4(outCol, 0.8);
  CalcBrightness(BrightColor, vec4(outCol, 1.0));
}