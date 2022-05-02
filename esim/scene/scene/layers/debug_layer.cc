#include "debug_layer.h"
#include "glapi/buffer_object.h"
#include "scene/programs/bounding_box_program.h"
#include <vector>

namespace esim {

namespace scene {

class debug_layer::impl {
public:
  inline std::vector<bounding_box_vertex> gen_axes_buffer() noexcept {
    using namespace glm;
    std::vector<bounding_box_vertex> buffer(6);
    auto it = buffer.begin();
    /// x-axis
    *it++ = bounding_box_vertex{trans::WGS84_A * 2.0f, 0.0f, 0.0f};
    *it++ = bounding_box_vertex{0.0f, 0.0f, 0.0f};

    /// y-axis
    *it++ = bounding_box_vertex{0.0f, trans::WGS84_A * 2.0f, 0.0f};
    *it++ = bounding_box_vertex{0.0f, 0.0f, 0.0f};
    
    /// z-axis
    *it++ = bounding_box_vertex{0.0f, 0.0f, trans::WGS84_A * 2.0f};
    *it++ = bounding_box_vertex{0.0f, 0.0f, 0.0f};
    
    return buffer;
  }
  
  inline void draw([[maybe_unused]] const camera &cmr) noexcept {
    using namespace glm;
    auto m = translate(mat4x4{1.0f}, static_cast<vec3>(- cmr.ecef()));
    // m = rotate(m, (float)(glfwGetTime()), vec3{0.f, 1.f, 1.f});
    // auto m = translate(mat4x4{1.0f}, static_cast<vec3>(cmr.ecef() - offset_));
    
    auto program = bounding_box_program::get();
    program->use();
    vbo_.bind();
    program->enable_position_pointer();
    program->bind_proj_uniform(cmr.projection());
    program->bind_view_uniform(cmr.view());
    program->bind_model_uniform(m);
    program->bind_color_uniform(vec4{1.0f, 0.0f, 0.0f, 1.0f});
    glDrawArrays(GL_LINES, 0, 2);
    program->bind_color_uniform(vec4{0.0f, 1.0f, 0.0f, 1.0f});
    glDrawArrays(GL_LINES, 2, 2);
    program->bind_color_uniform(vec4{0.0f, 0.0f, 1.0f, 1.0f});
    glDrawArrays(GL_LINES, 4, 2);
  }

  impl() noexcept {
    using namespace glm;
    vbo_.generate_buffer();
    vbo_.bind_buffer_data(gen_axes_buffer());
  }

private:
  gl::vertex_buffer<bounding_box_vertex> vbo_;
};

void debug_layer::draw(const camera &cmr) noexcept {
  assert(nullptr != pimpl_);
  pimpl_->draw(cmr);
}

debug_layer::debug_layer() noexcept : pimpl_{make_ptr_u<impl>()} {}

debug_layer::~debug_layer() noexcept {}

} // namespace scene

} // namespace esim
