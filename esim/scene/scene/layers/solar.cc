#include "solar.h"
#include "glapi/buffer_object.h"
#include "scene/programs/solar_program.h"
#include "transform/solar.h"
#include "transform/timing.h"
#include "transform/wgs84_decl.h"

namespace esim {

namespace scene {

class solar::impl {
public:
  inline std::vector<solar_vertex> gen_solor_vector() noexcept {
    using namespace glm;
    std::vector<solar_vertex> buffer(1);
    auto it = buffer.begin();
    double jd = trans::time::unix_to_juliandate(trans::time::current_utctimestamp());
    dvec3 solor_vec;
    trans::vector_to_solar(jd, solor_vec);
    (*it).x = static_cast<float>(solor_vec.x * trans::WGS84_A * 2.0);
    (*it).y = static_cast<float>(solor_vec.y * trans::WGS84_A * 2.0);
    (*it).z = static_cast<float>(solor_vec.z * trans::WGS84_A * 2.0);
    
    return buffer;
  }
  
  inline void draw([[maybe_unused]] const camera &cmr) noexcept {
    using namespace glm;
    auto m = translate(mat4x4{1.0f}, static_cast<vec3>(- cmr.ecef()));
    
    auto program = solar_program::get();
    program->use();
    vbo_.bind();
    program->enable_position_pointer();
    program->bind_proj_uniform(cmr.projection());
    program->bind_view_uniform(cmr.view());
    program->bind_model_uniform(m);
    program->bind_color_uniform(vec4{1.0f, 1.0f, 1.0f, 1.0f});
    glPointSize(10.0);
    glDrawArrays(GL_POINTS, 0, 1);
  }

  impl() noexcept {
    vbo_.generate_buffer();
    vbo_.bind_buffer_data(gen_solor_vector());
  }

private:
  gl::vertex_buffer<solar_vertex> vbo_;
};

void solar::draw(const camera &cmr) noexcept {
  assert(nullptr != pimpl_);
  pimpl_->draw(cmr);
}

solar::solar() noexcept : pimpl_{make_ptr_u<impl>()} {}

solar::~solar() noexcept {}

} // namespace scene

} // namespace esim
