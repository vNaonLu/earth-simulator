#version 460
precision highp float;

#define DECAY		  .974
#define EXPOSURE	.24
#define SAMPLES	   64
#define DENSITY	  .97
#define WEIGHT		.25

uniform layout(binding = 0) sampler2D u_Scene;
uniform layout(binding = 1) sampler2D u_OccludersSampler;
uniform vec2  u_LightNDCSunPos;
uniform vec2  u_Resolution;
uniform float u_Exposure;
uniform float u_Gamma;

in vec2 v_TexCoords;

out vec4 FragColor;

void NDCFixToZeroOne(out vec2 out_pixel, vec2 ndc) {
  out_pixel = (ndc + 1.0) / 2.0;
}

void CalcGodRay(out vec4 fragColor, in vec2 fragCoord) {
  float scale;
  vec4 occluders_color;
  vec2 coord, light;
  NDCFixToZeroOne(light, u_LightNDCSunPos.xy);
  coord = fragCoord.xy;

  occluders_color = texture(u_OccludersSampler, coord);
  float occ = occluders_color.r; //light

  vec2 dtc = (coord - light) * (1. / float(SAMPLES) * DENSITY);
  float illumdecay = 1.;

  for(int i = 0; i < SAMPLES; i++) {
    coord -= dtc;
    occluders_color = texture(u_OccludersSampler, coord);
    scale = occluders_color.r;
    scale *= illumdecay * WEIGHT;
    occ += scale;
    illumdecay *= DECAY;
  }

  // fragColor = vec4(vec3(0., obj * 0.333, 0.) + occ * EXPOSURE, 1.0);
  fragColor = vec4(texture(u_Scene, fragCoord).rgb + occ * EXPOSURE, 1.0);
}

void main() {
  // vec3 hdrColor = mix(texture(u_Scene, v_TexCoords).rgb, texture(u_FrameSampler, v_TexCoords).rgb, 0.5);
  // vec3 hdrColor = texture(u_Scene, v_TexCoords).rgb;

  // vec3 result = vec3(1.0) - exp(-hdrColor * u_Exposure);
  //      result = pow(result, vec3(1.0 / u_Gamma));

  // FragColor = vec4(texture(u_Scene, v_TexCoords).rgb, 1.0);
  CalcGodRay(FragColor, v_TexCoords);
}