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
    glBindTexture(GL_TEXTURE_2D, ids_[idx]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    return true;
  }

  return false;
}

inline texture::texture(size_t count) noexcept
    : ids_(count) {
  glGenTextures(static_cast<GLsizei>(count), ids_.data());
}

inline texture::~texture() noexcept {
  if (!ids_.empty()) {
    glDeleteTextures(static_cast<GLsizei>(ids_.size()), ids_.data());
    ids_.clear();
  }
}

} // namespace gl

} // namespace esim
