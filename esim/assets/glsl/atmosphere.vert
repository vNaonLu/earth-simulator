#version 460

precision highp float;

uniform vec3 u_CameraPos;
uniform float u_OuterRadius;
uniform float u_InnerRadius;
uniform vec3  u_InvWavelength;
uniform float u_ScaleOverScaleDepth;
uniform float u_Kr4PI;
uniform float u_Km4PI;
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

// void ONAS_CalcRayFromCamera(vec3 pos, out vec3 out_raySta, out vec3 out_rayDir);

// float ONAS_CalcRaySphereClosestInters(vec3 raySta, vec3 rayDir, vec3 sphereC, float sphereRSqr);

// float ONAS_CalcCamDistanceFromPlanetOrigin();

// float ONAS_Scale(float cosA);

// void main(void) {
//     // Get the ray from the camera to the vertex and its length
//   vec3 pos = (u_Modl * vec4(a_Pos, 1.0)).xyz + u_CameraPos;

//   vec3 raySta;
//   vec3 rayDir;
//   ONAS_CalcRayFromCamera(pos, raySta, rayDir);

//   vec3 start = raySta;

//   if(length(u_CameraPos) >= u_OuterRadius) {
//         // Calculate the farther intersection of the ray with the outer atmosphere
//         // (which is the far point of the ray passing through the atmosphere)
//     float near = ONAS_CalcRaySphereClosestInters(raySta, rayDir, vec3(0.0, 0.0, 0.0), u_OuterRadius * u_OuterRadius);

//     start += rayDir * near;
//   }

//     // Calculate attenuation from the camera to the top of the atmosphere toward the vertex
//   float height = length(start);
//   float useOuterRadius = ONAS_CalcCamDistanceFromPlanetOrigin();
//   float depth = exp(u_ScaleOverScaleDepth * (u_InnerRadius - useOuterRadius));
//   float angle = dot(rayDir, start) / height;
//   float scatter = depth * ONAS_Scale(angle);
//   gl_FrontSecondaryColor.rgb = exp(-scatter * (u_InvWavelength * u_Kr4PI + u_Km4PI));

//   mat4 mvp = u_Proj * u_View * u_Modl;
//   gl_Position = mvp * vec4(a_Pos, 1.0);
// }
