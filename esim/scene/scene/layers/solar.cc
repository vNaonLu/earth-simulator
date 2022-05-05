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
    std::vector<solar_vertex> buffer(2);
    auto it = buffer.begin();
    (*it).x = 1.0;
    (*it).y = 0.0;
    (*it++).z = 0.0;
    (*it).x = 0.0;
    (*it).y = 0.0;
    (*it).z = 0.0;
    
    return buffer;
  }
  
  inline void draw([[maybe_unused]] const rendering_infos &info) noexcept {
    using namespace glm;
    auto &cmr = info.camera;
    auto &sun = info.sun;
    auto m = translate(mat4x4{1.0f}, static_cast<vec3>(- cmr.ecef()));;
    m = scale(m, vec3{trans::WGS84_A * 3.0, trans::WGS84_A * 3.0, trans::WGS84_A * 3.0});
    m = sun.rotate_to_solar_direction(m);

    // m = scale(m, vec3{trans::WGS84_A * 3.0, trans::WGS84_A * 3.0, trans::WGS84_A * 3.0});
    
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
    glDrawArrays(GL_LINES, 0, 2);
  }

  impl() noexcept {
    vbo_.generate_buffer();
    vbo_.bind_buffer_data(gen_solor_vector());
  }

private:
  gl::vertex_buffer<solar_vertex> vbo_;
};

void solar::draw(const rendering_infos &info) noexcept {
  assert(nullptr != pimpl_);
  pimpl_->draw(info);
}

solar::solar() noexcept : pimpl_{make_ptr_u<impl>()} {}

solar::~solar() noexcept {}

} // namespace scene

} // namespace esim
