#version 460
precision highp float;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 FragOccluders;

uniform vec3 u_LightDir;
uniform float u_g;

in vec3 v_MieCol;
in vec3 v_RayleighCol;
in vec3 v_PosToCam;

float ONAS_CalcMiePhase(float cosA, float g);

void main() {

  float cosA = dot(-u_LightDir, v_PosToCam) / length(v_PosToCam);

  float miePhase = ONAS_CalcMiePhase(cosA, u_g);

  vec3 outCol = v_RayleighCol + miePhase * v_MieCol;

  FragColor = vec4(outCol, 0.8);
  FragOccluders = vec4(0.0, 0.0, 0.0, 1.0);
}