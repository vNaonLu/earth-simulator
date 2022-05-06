#version 330
precision highp float;
in vec3 vary_frag_pos;
in vec3 vary_normal;
in vec2 vary_tex_coord;
uniform vec3 unfm_solar_from;
uniform mat4 unfm_solar_rotation;
uniform sampler2D basemap;

vec3 compute_diffuse() {
  vec3 light_dir = -1.0 * unfm_solar_from;
  float diff = max(dot(normalize(vary_normal), light_dir), 0.0);

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
  vec3 base_color = texture(basemap, vary_tex_coord).xyz;
  gl_FragColor.rgb = compute_light(base_color);
  gl_FragColor.a = 1.0;
}