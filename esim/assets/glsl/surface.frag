#version 460
precision highp float;

uniform vec3 u_LightFrom;
uniform sampler2D u_BaseMap;

in vec3 v_Normal;
in vec2 v_TexCoord;

vec3 compute_diffuse() {
  vec3 light_dir = -1.0 * u_LightFrom;
  float diff = max(dot(normalize(v_Normal), light_dir), 0.0);

  return diff * vec3(1.0);
}

vec3 compute_ambient() {

  return vec3(1.0, 1.0, 1.0) * 0.1;
}

vec3 compute_light(const vec3 base_color) {
  vec3 ambient = compute_ambient();
  vec3 diffuse = compute_diffuse();

  return (ambient + diffuse) * base_color;
}

void main() {
  vec3 base_color = texture(u_BaseMap, v_TexCoord).xyz;
  gl_FragColor.rgb = compute_light(base_color);
  gl_FragColor.a = 1.0;
}