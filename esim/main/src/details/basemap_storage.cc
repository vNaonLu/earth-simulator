#include "basemap_storage.h"
#include <cassert>
#include <chrono>
#include <glm/gtx/string_cast.hpp>
#include <ostream>
#include <queue>
#include <regex>
#include <thread>

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>

namespace esim {

struct basemap::opaque {
  bool                            texture_created = {false};
  gl::texture                     texture;
  std::atomic<bool>               requested = {false};
  uptr<core::bitmap>              bitmap = {nullptr};
  std::future<uptr<core::bitmap>> bitmap_future;
};

bool basemap::is_ready() const noexcept {

  return nullptr != opaque_->bitmap;
}

bool basemap::is_requested() const noexcept {

  return opaque_->requested.load(std::memory_order_acquire);
}

void basemap::mark_requested() noexcept {
  opaque_->requested.store(true, std::memory_order_release);
}

void basemap::request(std::string host, std::string url) noexcept {
  assert(nullptr != opaque_);
  const static httplib::Headers headers = {
      {"Accept-Encoding", "gzip, deflate, br"},
      {"Connections", "keep-alive"},
      {"User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/101.0.4951.67 Safari/537.36"}
  };
  opaque_->bitmap_future = std::async(std::launch::async, [=]() -> uptr<core::bitmap> {
    httplib::SSLClient cli(host.data());
    cli.set_connection_timeout(std::chrono::seconds(10));
    if (auto res = cli.Get(url.data(), headers)) {
      auto request_data = make_uptr<core::bitmap>();
      request_data->load(res->body.data(), res->body.size());
      return request_data;
    } else {
      std::cerr << res.error() << std::endl;
      return nullptr;
    }
  });
}

void basemap::receive() noexcept {
  opaque_->bitmap = opaque_->bitmap_future.get();

  if (nullptr == opaque_->bitmap) {
    /// request again
    opaque_->requested.store(false, std::memory_order_release);
  }
}

rptr<const gl::texture> basemap::texture() noexcept {
  if (!opaque_->texture_created) {
    generate_texture();
  }
  return &opaque_->texture;
}

void basemap::generate_texture() noexcept {
  auto bitmap = opaque_->bitmap.get();
  if (nullptr != bitmap) {
    assert(opaque_->texture.load(*bitmap, 0, gl::texture::options{GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR}));
    opaque_->texture_created = true;
  }
}

basemap::basemap() noexcept
    : opaque_{make_uptr<opaque>()} {}

std::pair<rptr<basemap>, basemap_texinfo>
basemap_storage::get(const geo::maptile &tile, basemap_texinfo texinfo) noexcept {
  using namespace glm;
  if (tile.lod >= maps_.size()) {

    return get_from_parent(tile, texinfo);
  }

  auto &target = maps_[tile.lod][tile];
  if (nullptr == target) {
    target = make_uptr<basemap>();
  }

  if (!target->is_ready()) {
    if (!target->is_requested() && request_queue_.try_push(std::make_pair(target.get(), tile))) {
      target->mark_requested();
    }
    if (tile.lod == 0) {

      return std::make_pair(nullptr, texinfo);
    } else {

      return get_from_parent(tile, texinfo);
    }
  } else {
    
    return std::make_pair(target.get(), texinfo);
  }
}

bool basemap_storage::is_working() const noexcept {

  return is_working_.load(std::memory_order_acquire);
}

void basemap_storage::stop() noexcept {
  is_working_.store(false, std::memory_order_release);
}

basemap_storage::basemap_storage(std::string_view host, 
                                 std::string_view url_template,
                                 size_t max_lod) noexcept
    : host_{host}, url_template_{url_template}, request_queue_{16},
      maps_(max_lod), is_working_{true} {

  std::thread([=]() {
    const std::regex z("\\{z\\}"), x("\\{x\\}"), y("\\{y\\}");
    std::queue<rptr<basemap>> pending;
    std::pair<rptr<basemap>, geo::maptile> item;
    while (is_working()) {
      size_t pending_count = 0;

      while (request_queue_.try_pop(item)) {
        ++pending_count;
        auto &[node, tile] = item;
        std::string url = url_template_;
        url = std::regex_replace(url, z, std::to_string(tile.lod).c_str());
        url = std::regex_replace(url, x, std::to_string(tile.x).c_str());
        url = std::regex_replace(url, y, std::to_string(tile.y).c_str());
        node->request(host_, url);
        pending.emplace(node);
      }

      if (pending_count == 0) {
        std::this_thread::yield();
      } else {
        while (!pending.empty()) {
          auto node = pending.front(); pending.pop();
          node->receive();
        }
      }

    }
  }).detach();
}

basemap_storage::~basemap_storage() noexcept {
  stop();
}

std::pair<rptr<basemap>, basemap_texinfo> basemap_storage::get_from_parent(const geo::maptile &tile,
                                                                           basemap_texinfo texinfo) noexcept {
  using namespace glm;
  geo::maptile parent_tile{static_cast<uint8_t>(tile.lod - 1),
                           tile.x >> 1, tile.y >> 1};
  texinfo.scale *= 0.5f;
  texinfo.offset *= 0.5f;
  texinfo.offset += 0.5f * vec2(tile.y - (parent_tile.y << 1),
                                tile.x - (parent_tile.x << 1));

  return get(parent_tile, texinfo);
}

} // namespace esim
