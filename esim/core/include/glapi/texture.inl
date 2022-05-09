namespace esim {

namespace gl {

inline void texture::bind(size_t idx) const noexcept {
  assert(idx < ids_.size());
  glBindTexture(GL_TEXTURE_2D, ids_[idx]);
}

inline bool texture::load(std::string_view file, size_t idx) const noexcept {
  assert(idx < ids_.size());

  int width, height, nr_ch;
  unsigned char *data = stbi_load(file.data(), &width, &height, &nr_ch, 0);
  if (nullptr != data) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id_[idx]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    return true;
  }

  return false;
}

inline texture::textures(size_t count) noexcept
    : ids_(count) {
  glGenTextures(count, ids_.data());
}

inline texture::~texture() noexcept {
  if (!ids_.empty()) {
    glDeleteTextures(ids_.size(), ids_.data());
    ids_.clear();
  }
}

} // namespace gl

} // namespace esim
