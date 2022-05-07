#include "atmosphere.h"
#include "glapi/buffer_object.h"
#include "scene/programs/atmosphere_program.h"
#include "transform/wgs84.h"
#include <vector>

namespace esim {

namespace scene {

class atmosphere::impl {
public:

  inline void draw(const rendering_infos &info) noexcept {
    using namespace glm;
    auto &cmr = info.camera;
    auto &sun = info.sun;
    auto program = atmosphere_program::get();
    program->use();
    ebo_.bind();
    vbo_.bind();
    program->enable_position_pointer();
    program->bind_model_uniform(cmr.model(offset_));
    program->bind_view_uniform(cmr.view());
    program->bind_proj_uniform(cmr.projection());
    program->bind_view_depth_uniform(250'000);
    program->bind_scatter_coefficient_uniform(vec4{0.1981, 0.4656, 0.8625, 0.75});
    program->bind_solor_dir_uniform(static_cast<vec3>(sun.direction()));
    // glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(ebo_.size()), GL_UNSIGNED_INT, nullptr);
    glDrawElements(GL_LINES, static_cast<GLsizei>(ebo_.size()), GL_UNSIGNED_INT, nullptr);
    ESIM_GL_TRACE();
  }

  impl() noexcept : offset_{0.0f} {
    vbo_.generate_buffer();
    vbo_.bind_buffer_data(gen_vertex_buffer());

    ebo_.generate_buffer();
    ebo_.bind_buffer_data(gen_element_buffer());
  }

  ~impl() noexcept {}

private:
  constexpr static size_t latitude_details = 23;
  constexpr static size_t longitude_details = 23;
  constexpr static double max_latitude = +90.0;
  constexpr static double min_latitude = -90.0;
  constexpr static double stride_latitude = (max_latitude - min_latitude) / latitude_details;
  constexpr static double max_longitude = +180.0;
  constexpr static double min_longitude = -180.0;
  constexpr static double stride_longitude = (max_longitude - min_longitude) / longitude_details;
  
  inline uint32_t to_vertex_index(size_t row, size_t col) const noexcept {
    return static_cast<uint32_t>(row * (longitude_details) + col);
  }

  inline std::vector<atmosphere_vertex> gen_vertex_buffer() noexcept {
    using namespace glm;
    std::vector<atmosphere_vertex> buffer((latitude_details + 1) * (longitude_details + 1));
    auto it = buffer.begin();
    for (size_t i = 0; i <= latitude_details; ++i) {
      for (size_t j = 0; j <= longitude_details; ++j) {
        auto &v = (*it++).pos;
        dvec3 ecef{radians(min_latitude + stride_latitude * i),
                   radians(min_longitude + stride_longitude * j),
                   100'000};
        trans::wgs84geo_to_ecef(ecef, ecef);
        v.x = static_cast<float>(ecef.x);
        v.y = static_cast<float>(ecef.y);
        v.z = static_cast<float>(ecef.z);
        if (j != longitude_details) {
          /// avoid duplicatly computing same point
          offset_ += v;
        }
      }
    }
    
    offset_ /= latitude_details * longitude_details;

    for (auto &v : buffer) {
      v.pos -= static_cast<vec3>(offset_);
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
  glm::dvec3 offset_;
  gl::index_buffer<uint32_t> ebo_;
  gl::vertex_buffer<atmosphere_vertex> vbo_;
};

void atmosphere::draw(const rendering_infos &info) noexcept {
  assert(nullptr != pimpl_);
  pimpl_->draw(info);
}

atmosphere::atmosphere() noexcept : pimpl_{make_ptr_u<impl>()} {}

atmosphere::~atmosphere() noexcept {}

} // namespace scene

} // namespace esim
