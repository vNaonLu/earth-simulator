#ifndef __ESIM_ESIM_SOURCE_BASEMAP_STORAGE_H_
#define __ESIM_ESIM_SOURCE_BASEMAP_STORAGE_H_

#include "core/bitmap.h"
#include "core/fifo.h"
#include "core/transform.h"
#include "core/utils.h"
#include "glapi/texture.h"
#include <atomic>
#include <glm/vec4.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace esim {

class basemap {
public:
  bool is_ready() const noexcept;

  bool is_requested() const noexcept;

  void mark_requested() noexcept;

  void blocking_request(std::string host, std::string url) noexcept;

  const gl::texture &texture() noexcept;

  basemap() noexcept;

  ~basemap() = default;

private:
  void generate_texture() noexcept;

private:
  std::atomic<bool> requested_, responsed_;
  bool              texture_created_;
  core::bitmap      bitmap_;
  gl::texture       texture_;
};

struct basemap_texinfo {
  float     scale;
  glm::vec2 offset;
};

class basemap_storage final {
public:
  std::pair<rptr<basemap>, basemap_texinfo> get(const geo::maptile &tile,
                                                basemap_texinfo texinfo = basemap_texinfo{1.0f, glm::vec2{0.0}}) noexcept;

  bool is_working() const noexcept;

  void stop() noexcept;

  basemap_storage(std::string_view host, std::string_view url_template, size_t max_lod) noexcept;

  ~basemap_storage() noexcept;

private:
  std::string               host_, url_template_;
  core::fifo<std::pair<rptr<basemap>, geo::maptile>>           request_queue_;
  std::vector<std::unordered_map<geo::maptile, uptr<basemap>>> maps_;
  std::atomic<bool>         is_working_;
};

} // namespace esim

#endif