#include "outer_space.h"
#include "atmosphere.h"
#include "glapi/buffer_object.h"
#include "glapi/texture.h"
#include "scene/programs/outer_space_program.h"
#include "transform/wgs84.h"
#include <vector>

namespace esim {

namespace scene {

class outer_space::impl {
public:
  inline void draw(const rendering_infos &info) noexcept {
    using namespace glm;
    auto program = outer_space_program::get();

    program->use();
    spacemap_.bind();
    ebo_.bind();
    vbo_.bind();
    program->enable_position_pointer();
    program->enable_texture_coord_pointer();
    program->bind_common_uniform(info);
    // program->bind_model_uniform(mat4x4{1.0f});
    program->bind_model_uniform(GC_rotation_);

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(ebo_.size()), GL_UNSIGNED_INT, nullptr);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
  }

  impl() noexcept
      : GC_rotation_{to_galactic_center()},
        offset_{0.0f} {
    vbo_.generate_buffer();
    vbo_.bind_buffer_data(gen_vertex_buffer());

    ebo_.generate_buffer();
    ebo_.bind_buffer_data(gen_element_buffer());
    
    spacemap_.generate();
    spacemap_.bind_from_file("img/starmap_g4k.jpg");
  }

  ~impl() noexcept {}

private:
  constexpr static size_t latitude_details = 22;
  constexpr static size_t longitude_details = 45;
  // constexpr static size_t latitude_details = 10;
  // constexpr static size_t longitude_details = 10;
  constexpr static double max_latitude = +90.0;
  constexpr static double min_latitude = -90.0;
  constexpr static double stride_latitude = (max_latitude - min_latitude) / latitude_details;
  constexpr static double max_longitude = +180.0;
  constexpr static double min_longitude = -180.0;
  constexpr static double stride_longitude = (max_longitude - min_longitude) / longitude_details;
  
  inline uint32_t to_vertex_index(size_t row, size_t col) const noexcept {

    return static_cast<uint32_t>(row * (longitude_details) + col);
  }

  inline static glm::mat4x4 to_galactic_center() noexcept {
    /// from equinox to galactic center
    using namespace glm;
    /// galactic center locate at
    /// 17h 45m 40.0409s, −29° 00′ 28.118″ (J2000) By Wikipedia
    float RA = radians((17.0f * 15.0f) + (45.0f * (15.0f / 60.0f)) + (40.0409f * (15.0f / 60.0f / 60.0f)));
    float Dc = radians(-29.0f + (0.0f / 60.0f) + (28.118f / 60.0f / 60.0f));
    vec3  eqinox = normalize(vec3(1.0f, 0.0f, 0.0f));
    vec3  gal_center = normalize(vec3{cos(Dc) * cos(RA), cos(Dc) * sin(RA), sin(Dc)});
    vec3  rotation_axis = cross(eqinox, gal_center);

    std::cout << glm ::to_string(gal_center) << std::endl;

    return rotate(mat4x4{1.0f}, acos(dot(eqinox, gal_center)), rotation_axis);
  }

  inline std::vector<outer_space_vertex> gen_vertex_buffer() noexcept {
    using namespace glm;
    std::vector<outer_space_vertex> buffer((latitude_details + 1) * (longitude_details + 1));
    auto it = buffer.begin();
    float radius = sqrt(2.0f);
    for (size_t i = 0; i <= latitude_details; ++i) {
      for (size_t j = 0; j <= longitude_details; ++j) {
        auto &v = (*it).pos;
        auto &tx = (*it++).texcoord;
        dvec3 pos{radians(min_latitude + stride_latitude * i),
                  radians(min_longitude + stride_longitude * j),
                  0.0};
        double coslat = cos(pos.x), sinlat = sin(pos.x),
               coslon = cos(pos.y), sinlon = sin(pos.y);
        v.x = static_cast<float>(radius * coslat * coslon);
        v.y = static_cast<float>(radius * coslat * sinlon);
        v.z = static_cast<float>(radius * sinlat);
        tx.s = static_cast<float>((pos.y - radians(min_longitude)) / 2.0f / pi<double>());
        tx.t = static_cast<float>((pos.x - radians(min_latitude)) / pi<double>());
        offset_ += v;
      }
    }
    offset_ /= ((latitude_details + 1) * (longitude_details + 1));
    for(auto &v : buffer) {
      v.pos -= offset_;
    }

    return buffer;
  }
  
  inline std::vector<uint32_t> gen_element_buffer() noexcept {
    using namespace glm;
    std::vector<uint32_t> buffer((latitude_details + 1) * longitude_details * 6);
    auto it = buffer.begin();
    for (size_t i = 0; i <= latitude_details; ++i) {
      for (size_t j = 0; j < longitude_details; ++j) {
        *it++ = to_vertex_index(i  , j  );
        *it++ = to_vertex_index(i+1, j+1);
        *it++ = to_vertex_index(i  , j+1);
        *it++ = to_vertex_index(i  , j  );
        *it++ = to_vertex_index(i+1, j  );
        *it++ = to_vertex_index(i+1, j+1);
      }
    }

    return buffer;
  }

private:
  const glm::mat4x4                     GC_rotation_;
  glm::dvec3                            offset_;
  gl::index_buffer<uint32_t>            ebo_;
  gl::vertex_buffer<outer_space_vertex> vbo_;
  gl::texture                           spacemap_;
};

void outer_space::draw(const rendering_infos &info) noexcept {
  assert(nullptr != pimpl_);
  pimpl_->draw(info);
}

outer_space::outer_space() noexcept : pimpl_{make_ptr_u<impl>()} {}

outer_space::~outer_space() noexcept {}

} // namespace scene

} // namespace esim
