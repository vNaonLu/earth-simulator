#version 460

precision highp float;

uniform vec3 u_LightDir;
uniform float u_g;

in vec3 v_MieCol;
in vec3 v_RayleighCol;
in vec3 v_PosToCam;

float ONAS_CalcMiePhase(float cosA, float g);

void main() {

  float cosA = dot(u_LightDir, v_PosToCam) / length(v_PosToCam);

  float miePhase = ONAS_CalcMiePhase(cosA, u_g);

  vec3 outCol = v_RayleighCol + miePhase * v_MieCol;

  gl_FragColor = vec4(outCol, 0.9);
}

// void main (void)
// {
//     // gl_FragColor = gl_SecondaryColor * texture2D(s2Test, gl_TexCoord[0].st);
//     gl_FragColor = gl_SecondaryColor;
// }
