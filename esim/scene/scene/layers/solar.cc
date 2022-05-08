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
    auto m = translate(mat4x4{1.0f}, static_cast<vec3>(- cmr.ecef()));
    m = scale(m, vec3{static_cast<float>(trans::astronomical_unit())});
    m = sun.rotate_to_solar_direction(m);

    auto program = solar_program::get();
    program->use();
    vbo_.bind();
    program->enable_position_pointer();
    program->bind_proj_uniform(cmr.projection());
    program->bind_view_uniform(cmr.view());
    program->bind_model_uniform(m);
    program->bind_color_uniform(vec4{1.0f, 1.0f, 1.0f, 1.0f});
    glPointSize(2 * sun.visual_pixel_size_of_sun(cmr));

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDrawArrays(GL_POINTS, 0, 1);
    program->bind_color_uniform(vec4{1.0f, 1.0f, 1.0f, 0.2f});
    glDrawArrays(GL_LINES, 0, 2);
    glDepthMask(GL_TRUE);

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
