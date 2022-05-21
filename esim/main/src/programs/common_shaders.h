#ifndef __ESIM_MAIN_SOURCE_SCENE_PROGRAMS_COMMON_SHADERS_H_
#define __ESIM_MAIN_SOURCE_SCENE_PROGRAMS_COMMON_SHADERS_H_

#include "core/utils.h"
#include "glapi/shader.h"
#include <glad/glad.h>

namespace esim {

namespace program {

template <GLenum vertex_type>
class common_shader : public gl::shader {
public:
  inline static rptr<common_shader> get() noexcept {
    static uptr<common_shader> single_;
    if (nullptr == single_) {
      single_ = make_uptr<common_shader>();
      single_->compile_from_file("assets/glsl/common.glsl");
    }

    return single_.get();
  }

  common_shader() noexcept : shader{vertex_type} {}

  ~common_shader() = default;
};

} // namespace program

} // namespace esim

#endif