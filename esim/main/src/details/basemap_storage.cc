#include "basemap_storage.h"
#include <cassert>
#include <ostream>
#include <regex>
#include <thread>

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>

namespace esim {

bool basemap::is_ready() const noexcept {

  return responsed_.load(std::memory_order_acquire);
}

bool basemap::is_requested() const noexcept {

  return requested_.load(std::memory_order_acquire);
}

void basemap::mark_requested() noexcept {
  requested_.store(true, std::memory_order_release);
}

void basemap::blocking_request(std::string host, std::string url) noexcept {
  const static httplib::Headers header = {
      {"Accept-Encoding", "gzip, deflate, br"},
      {"Connections", "keep-alive"},
      {"User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/101.0.4951.67 Safari/537.36"}
  };
  httplib::SSLClient cli(host.data());
  cli.set_connection_timeout(std::chrono::seconds(10));
  if (auto res = cli.Get(url.data(), header)) {
    assert(bitmap_.load(res->body.data(), res->body.size()));
  } else {
    std::cout << res.error() << std::endl;
  }
  responsed_.store(true, std::memory_order_release);
}

const gl::texture &basemap::texture() noexcept {
  if (!texture_created_) {
    generate_texture();
  }
  return texture_;
}

void basemap::generate_texture() noexcept {
  assert(texture_.load(bitmap_));
  texture_created_ = true;
}

basemap::basemap() noexcept : requested_{false}, responsed_{false}, texture_created_{false} {}

rptr<basemap> basemap_storage::get(const geo::maptile &tile) noexcept {
  assert(tile.lod < maps_.size());
  auto &target = maps_[tile.lod][tile];
  if (nullptr == target) {
    target = make_uptr<basemap>();
  }

  if (!target->is_ready()) {
    if (!target->is_requested()) {
      request_queue_.push(std::make_pair(target.get(), tile));
      target->mark_requested();
    }

    return nullptr;
  } else {

    return target.get();
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
    : host_{host}, url_template_{url_template}, request_queue_{64},
      maps_(max_lod), is_working_{true} {

  std::thread([=]() {
    const std::regex z("\\{z\\}"), x("\\{x\\}"), y("\\{y\\}");
    while (is_working()) {
      std::pair<rptr<basemap>, geo::maptile> item;
      if (request_queue_.try_pop(item)) {
        auto &[node, tile] = item;
        std::ostringstream url;
        std::regex_replace(std::ostreambuf_iterator<char>(url), url_template_.begin(), url_template_.end(), z, std::to_string(tile.lod).c_str());
        std::regex_replace(std::ostreambuf_iterator<char>(url), url_template_.begin(), url_template_.end(), x, std::to_string(tile.x).c_str());
        std::regex_replace(std::ostreambuf_iterator<char>(url), url_template_.begin(), url_template_.end(), y, std::to_string(tile.y).c_str());
        node->blocking_request(host_, url.str());
      } else {
        std::this_thread::yield();
      }
    }
  }).detach();
}

basemap_storage::~basemap_storage() noexcept {
  stop();
}

} // namespace esim
