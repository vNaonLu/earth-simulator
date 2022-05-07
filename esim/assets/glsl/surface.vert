#version 460
precision highp float;

uniform mat4 u_Modl;
uniform mat4 u_View;
uniform mat4 u_Proj;

in vec3 a_Pos;
in vec3 a_Normal;
in vec2 a_TexCoord;

out vec3 v_Normal;
out vec2 v_TexCoord;

void main (void) {
  mat4 mvp = u_Proj * u_View * u_Modl;
  v_TexCoord = a_TexCoord;
  v_Normal = (u_Modl * vec4(a_Normal, 0.0)).xyz;
  gl_Position = mvp * vec4(a_Pos, 1.0);
}


// varying vec3 v_groundCol;
// varying vec3 v_attenuation;

// void main(void)
// {
//     // Get the ray from the camera to the vertex and its length
//     //  (which is the far point of the ray passing through the atmosphere)
//     vec3 pos = gl_Vertex.xyz;

//     if ( length( u_CameraPos ) >= u_OuterRadius )
//         ONAS_CalcColorsForGroundOutside( v_groundCol, v_attenuation, pos );
//     else
//         ONAS_CalcColorsForGroundInside( v_groundCol, v_attenuation, pos );

//     gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
//     gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
//     gl_TexCoord[1] = gl_TextureMatrix[1] * gl_MultiTexCoord1;
// }
