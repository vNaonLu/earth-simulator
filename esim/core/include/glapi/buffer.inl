namespace esim {

namespace gl {

template <typename underlying_type>
inline const std::vector<underlying_type> &buffer<underlying_type>::data(size_t idx) const noexcept {
  assert(idx <= data_.size());
  return data_[idx];
}

template <typename underlying_type>
inline size_t buffer<underlying_type>::size(size_t idx) const noexcept {

  return data(idx).size();
}

template <typename underlying_type>
inline void buffer<underlying_type>::bind(size_t idx) noexcept {
  assert(idx <= data_.size());
  glBindBuffer(type(), ids_[idx]);
}

template <typename underlying_type>
inline void buffer<underlying_type>::unbind() const noexcept {
  glBindBuffer(type(), 0);
}

template <typename underlying_type>
inline void buffer<underlying_type>::bind_buffer(std::vector<underlying_type> &&data,
                                                 GLenum usage, size_t idx) noexcept {
  assert(idx <= data_.size());
  bind(idx);
  auto &buffer = data_.at(idx) = std::move(data);
  glBufferData(type(), sizeof(underlying_type) * buffer.size(), buffer.data(), usage);
}

template <typename underlying_type>
inline GLenum buffer<underlying_type>::type() const noexcept {

  return buffer_type_;
}

template <typename underlying_type>
inline buffer<underlying_type>::buffer(GLenum type, size_t count) noexcept
    : buffer_size_{count}, buffer_type_{type},
      ids_(buffer_size_, 0), data_(buffer_size_) {
  glGenBuffers(static_cast<GLsizei>(buffer_size_), ids_.data());
}

template <typename underlying_type>
inline buffer<underlying_type>::~buffer() noexcept {
  if (!ids_.empty()) {
    glDeleteBuffers(static_cast<GLsizei>(buffer_size_), ids_.data());
    ids_.clear();
  }
}

} // namespace gl

} // namespace esim
