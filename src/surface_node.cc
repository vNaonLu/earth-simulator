#include "surface_node.h"
#include "coord/transform.h"
#include <fstream>
#include <functional>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

int idx = 0;
int curr = -1;
auto file = std::fopen("out.txt", "w");

#define TRACE_GL()                                                                     \
  switch (glGetError()) {                                                              \
                                                                                       \
  case GL_INVALID_ENUM:                                                                \
    std::cout << __LINE__ << " error:  GL_INVALID_ENUM" << std::endl;                  \
    break;                                                                             \
  case GL_INVALID_VALUE:                                                               \
    std::cout << __LINE__ << " error:  GL_INVALID_VALUE" << std::endl;                 \
    break;                                                                             \
  case GL_INVALID_OPERATION:                                                           \
    std::cout << __LINE__ << " error:  GL_INVALID_OPERATION" << std::endl;             \
    break;                                                                             \
  case GL_INVALID_FRAMEBUFFER_OPERATION:                                               \
    std::cout << __LINE__ << " error:  GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl; \
    break;                                                                             \
  case GL_OUT_OF_MEMORY:                                                               \
    std::cout << __LINE__ << " error:  GL_OUT_OF_MEMORY" << std::endl;                 \
    break;                                                                             \
  }

namespace esim {

class surface_node::impl {
public:
  struct vertex {
    float x, y, z;

  };
  
  struct compute_vertex {
    double x, y, z;
  };

  inline static size_t to_index(size_t row, size_t col) noexcept {
    return row * details + col;
  }

  inline static size_t to_vertex_index(size_t row, size_t col) noexcept {
    return row * (details) + col;
  }

  bool is_ready() const noexcept {

    return nullptr != pprog_ && pprog_->is_ready();
  }

  void prepare_indices() noexcept {
    const uint32_t vertex_count = (details) * (details);
    indices_buf_.resize(vertex_count * 6);

    size_t idx = 0;
    for (size_t i = 0; i < details; ++i) {
      for (size_t j = 0; j < details; ++j) {
        indices_buf_[idx++] = to_index(i  , j  );
        indices_buf_[idx++] = to_index(i+1, j+1);
        indices_buf_[idx++] = to_index(i  , j+1);
        indices_buf_[idx++] = to_index(i  , j  );
        indices_buf_[idx++] = to_index(i+1, j  );
        indices_buf_[idx++] = to_index(i+1, j+1);
      }
    }

    glGenBuffers(1, &indices_id_);
    TRACE_GL();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id_);
    TRACE_GL();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(uint32_t) * vertex_count * 6,
                 indices_buf_.data(), GL_STATIC_DRAW);
    TRACE_GL();
  }

  void prepare_vertex() noexcept {
    const double tile_step = 1 / (double)details;
    const uint32_t zoom = tile_.zoom;
    const uint32_t vertex_count = (details + 1) * (details + 1);
    double xtile = tile_.xtile,
           ytile = tile_.ytile;

    std::memset(&offset_, 0, sizeof(offset_));
    pos_buf_.resize(vertex_count);
    std::vector<compute_vertex> computing_vertex(vertex_count);
    printf("(%d, %lf, %lf)\n", zoom, xtile, ytile);
    for (size_t i=0; i<=details; ++i) {
      for (size_t j=0; j<=details; ++j) {
        auto &buffer = computing_vertex[to_index(i, j)];
        buffer.x = coord::tiley_to_lati(zoom, ytile + tile_step * i);
        buffer.y = coord::tilex_to_long(zoom, xtile + tile_step * j);
        printf("%d : {%f, %f} lla: {%lf, %lf}",to_index(i,j), ytile + tile_step * i, xtile + tile_step * j,buffer.x, buffer.y);
        buffer.z = 0.0;
        coord::geodetic_to_ecef<coord::TEST>(buffer.x, buffer.y, buffer.z);
        printf(" pos: {%lf, %lf}\n\n",buffer.x, buffer.y);
        offset_.x += buffer.x;
        offset_.y += buffer.y;
        offset_.z += buffer.z;
      }
    }

    offset_.x /= vertex_count;
    offset_.y /= vertex_count;
    offset_.z /= vertex_count;

    auto v_it = computing_vertex.begin();
    for (auto &v : pos_buf_) {
      auto &compute_v = *v_it++;
      v.x = (float) compute_v.x - offset_.x;
      v.y = (float) compute_v.y - offset_.y;
      v.z = (float) compute_v.z - offset_.z;
      // v.x = compute_v.x;
      // v.y = compute_v.y;
      // v.z = compute_v.z;
    }

    atb_posn_ = pprog_->obtain_attribute("attb_pos");
    glGenBuffers(1, &h_pos_buf_);
    TRACE_GL();
    glBindBuffer(GL_ARRAY_BUFFER, h_pos_buf_);
    TRACE_GL();
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * pos_buf_.size(), pos_buf_.data(), GL_STATIC_DRAW);
    TRACE_GL();
    glEnableVertexAttribArray(atb_posn_);
    glVertexAttribPointer(atb_posn_, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
    TRACE_GL();
  }

  void draw(int width, int height) noexcept {
    const uint32_t vertex_count = details * details;
    GLfloat ratio = (GLfloat)width / (GLfloat)height;

    // auto m = glm::mat4x4(1.f);
    auto m = glm::rotate(glm::mat4x4(1.f), glm::radians((float)glfwGetTime() * -5), glm::vec3(1.f, 1.f ,0.f));
    // auto m = glm::rotate(glm::mat4x4(1.f), 0.f, glm::vec3(1.f, 1.f ,0.f));
    auto v = glm::mat4x4(1.f);
    auto p = glm::mat4x4(1.f);
    // auto p = glm::perspective(45.f, ratio, 1.0f, 150.f);
    // auto offset = glm::vec3(offset_.x, offset_.y, offset_.z);
    auto offset = glm::vec3(0.f, 0.f, 0.f);

    pprog_->use_program();
    glUniformMatrix4fv(unf_modl_, 1, GL_FALSE, glm::value_ptr(m));
    TRACE_GL();
    glUniformMatrix4fv(unf_view_, 1, GL_FALSE, glm::value_ptr(v));
    TRACE_GL();
    glUniformMatrix4fv(unf_proj_, 1, GL_FALSE, glm::value_ptr(p));
    TRACE_GL();
    glUniform3fv(unf_offset_, 1, glm::value_ptr(offset));
    TRACE_GL();

    glUniform3fv(unf_offset_, 1, glm::value_ptr(glm::vec3(0.f, 0.6f, 0.f)));
    glPointSize(10);
    TRACE_GL();
    glUniform3fv(unf_offset_, 1, glm::value_ptr(glm::vec3(0.2f, 0.2f, 0.2f)));
    glDrawElements(GL_TRIANGLES, indices_buf_.size(), GL_UNSIGNED_INT, nullptr);
    glUniform3fv(unf_offset_, 1, glm::value_ptr(glm::vec3(0.5f, 0.5f, 0.5f)));
    glDrawArrays(GL_POINTS, 0, vertex_count);
    if (idx < 0) {
      idx += vertex_count * 6;
    }
    int x = (idx * 3) % (vertex_count * 6);
    if (curr != x) {
      curr = x;
      std::cout << "indices[" << x << "] = " << (int)indices_buf_[curr] << " => ";
      std::cout << pos_buf_[indices_buf_[curr]].x << ", " << pos_buf_[indices_buf_[curr]].y << ", " << pos_buf_[indices_buf_[curr]].z  << std::endl;
      std::cout << "indices[" << x+1 << "] = " << (int)indices_buf_[curr+1] <<  " => ";
      std::cout << pos_buf_[indices_buf_[curr+1]].x << ", " << pos_buf_[indices_buf_[curr+1]].y<< ", " << pos_buf_[indices_buf_[curr+1]].z  << std::endl;
      std::cout << "indices[" << x+2 << "] = " << (int)indices_buf_[curr+2] <<  " => ";
      std::cout << pos_buf_[indices_buf_[curr+2]].x << ", " << pos_buf_[indices_buf_[curr+2]].y << ", " << pos_buf_[indices_buf_[curr+2]].z << std::endl << std::endl;
    }
    glUniform3fv(unf_offset_, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
    glDrawElements(GL_LINE_STRIP, indices_buf_.size(), GL_UNSIGNED_INT, nullptr);
    glUniform3fv(unf_offset_, 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 1.0f)));
    glDrawElements(GL_POINTS, 3, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * curr));
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * curr));
    TRACE_GL();
  }

  impl(uint32_t zoom, uint32_t xtile, uint32_t ytile,
       gl_error_callback cb) noexcept
      : tile_{zoom, xtile, ytile},
        pprog_{nullptr},
        error_msg_{cb},
        offset_{0.0L, 0.0L, 0.0L} {
    std::string vs_text, fs_text;

    if (!read_file("glsl/surface.vs", vs_text)) {
      error("file not found: glsl/surface.vs");
      return;
    }
    
    if (!read_file("glsl/surface.fs", fs_text)) {
      error("file not found: glsl/surface.fs");
      return;
    }

    pprog_ = make_ptr_u<gl_program>(vs_text, fs_text, error_msg_);
    if (!pprog_->is_ready()) {
      error("surface_node initialized failed");
      return;
    }

    unf_modl_ = pprog_->obtain_uniform("unfm_model");
    unf_view_ = pprog_->obtain_uniform("unfm_view");
    unf_proj_ = pprog_->obtain_uniform("unfm_proj");
    unf_offset_ = pprog_->obtain_uniform("unfm_offset");
    prepare_vertex();
    
    if (indices_buf_.empty()) {
      prepare_indices();
    }
  }

  ~impl() = default;

private:
  static bool read_file(std::string_view path, std::string &buf) noexcept {
    std::ifstream file(path.data());
    if (file.is_open()) {
      buf = std::string(std::istreambuf_iterator<char>(file),
                        std::istreambuf_iterator<char>());

      return true;
    }

    return false;
  }

  void error(const char *msg) const {
    if (nullptr != error_msg_) {
      error_msg_(msg);
    }
  }

private:
  static const uint32_t details = 20;
  const coord::tile_number tile_;
  std::vector<uint32_t> indices_buf_;
  GLuint indices_id_;
  u_ptr<gl_program> pprog_;
  gl_error_callback error_msg_;

  GLint unf_modl_, unf_view_, unf_proj_, unf_offset_;
  GLint atb_posn_;
  GLuint h_pos_buf_;
  compute_vertex offset_;
  std::vector<vertex> pos_buf_;
};

bool surface_node::is_ready() const noexcept {

  return nullptr != pimpl && pimpl->is_ready();
}

void surface_node::draw(int width, int height) noexcept {
  if (nullptr != pimpl) {
    pimpl->draw(width, height);
  }
}

surface_node::surface_node(uint32_t zoom, uint32_t xtile, uint32_t ytile,
                           gl_error_callback cb) noexcept
    : pimpl{new impl(zoom, xtile, ytile, cb)} {
}

} // namespace esim
