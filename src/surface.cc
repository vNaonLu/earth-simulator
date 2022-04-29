#include "surface.h"
#include "glapi/program.h"
#include <fstream>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/mat4x4.hpp>
#include <string>

#include <iostream>
#include <vector>

namespace esim {

static const struct {
  float x, y;
  float r, g, b;
} vertices[4] = {{-0.6f, -0.4f, 1.f, 0.f, 0.f},
                 {0.6f, -0.4f,  0.f, 1.f, 0.f},
                 {0.f,   0.6f,  0.f, 0.f, 1.f}};

class surface::impl {
public:
  inline void draw(int width, int height) noexcept {
    float ratio = width / (float)height;

    auto m = glm::rotate(glm::mat4x4(1.f), (float)glfwGetTime() * 10, glm::vec3(0.f, 0.f, 1.f));
    auto p = glm::ortho(-1.f, 1.f, -1.f, 1.f, 1.f, -1.f);
    // auto p = glm::perspective(glm::radians(45.0f), ratio, -1.f, 1.f);
    auto mvp = p * m;

    pprog->use_program();
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));
    glDrawArrays(GL_TRIANGLES, 0, 3);
  }

  inline bool is_ready() const noexcept {

    return nullptr != pprog && pprog->is_ready();
  } 

  impl() noexcept : pprog{nullptr} {
    std::string vert_text, frag_text;
    {
      std::ifstream file("glsl/surface.vs");
      if (file.is_open()) {
        vert_text = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
      } else {
        return;
      }
    }

    {
      std::ifstream file("glsl/surface.fs");
      if (file.is_open()) {
        frag_text = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
      } else {
        return;
      }
    }

    pprog = make_ptr_u<gl_program>(vert_text, frag_text, [](const char *msg) {
      std::cout << msg << std::endl;
    });

    glGenBuffers(1, &vert_buf);
    glBindBuffer(GL_ARRAY_BUFFER, vert_buf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    mvp_location = pprog->obtain_uniform("mvp");
    vpos_location = pprog->obtain_attribute("v_pos");
    vcol_location = pprog->obtain_attribute("v_col");

    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (void*)0);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (void*)(2 * sizeof(float)));

  }

  ~impl() noexcept {
    glDeleteBuffers(1, &vert_buf);
  }

private:
  u_ptr<gl_program> pprog;
  GLuint vert_buf;
  GLint mvp_location, vpos_location, vcol_location;

};

surface::surface() noexcept : pimpl{std::make_unique<impl>()} {}

surface::~surface() noexcept {}

bool surface::is_ready() noexcept {

  return nullptr != pimpl && pimpl->is_ready(); 
}

bool surface::draw(int width, int height) noexcept {
  if (nullptr != pimpl) {
    pimpl->draw(width, height);

    return true;
  }

  return false;
}

} // namespace esim
