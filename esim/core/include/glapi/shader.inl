namespace esim {

namespace gl {

inline bool shader::compile(std::string_view content) noexcept {
  const GLint text_size = (GLint)content.size();
  rptr<const GLchar> data = content.data();
  glShaderSource(id_, 1, &data, &text_size);
  glCompileShader(id_);

  GLint code;
  glGetShaderiv(id_, GL_COMPILE_STATUS, &code);
  if (code == GL_FALSE) {
    GLint max_length = 0;
    glGetShaderiv(id_, GL_INFO_LOG_LENGTH, &max_length);
    auto msg = make_uptr<char[]>(max_length + 1);
    glGetShaderInfoLog(id_, max_length, &max_length, msg.get());
    glDeleteShader(id_);
    id_ = 0;

    std::cout << msg << std::endl;

    return false;
  }

  return true;
}

inline bool shader::compile_from_file(std::string_view file) noexcept {
  auto content = read(file);

  return compile(content);
}

inline GLenum shader::type() const noexcept {

  return shader_type_;
}

inline GLuint shader::id() const noexcept {

  return id_;
}

inline void shader::include(rptr<shader> dependency) noexcept {
  assert(shader_type_ == dependency->type());
  dependencies_.insert(dependency);
}

inline std::unordered_set<GLuint> shader::dependencies() const noexcept {
  std::unordered_set<GLuint> ids;
  for (auto &so : dependencies_) {
    ids.insert(so->id());
  }

  return ids;
}

inline shader::shader(GLenum type) noexcept
    : shader_type_{type}, id_{glCreateShader(type)} {
}

inline shader::~shader() noexcept {
  if (id_ != 0) {
    glDeleteShader(id_);
    id_ = 0;
  }
}

inline std::string shader::read(std::string_view file) noexcept {
  std::ifstream f(file.data());
  assert(f.is_open());
  return std::string(std::istreambuf_iterator<char>(f),
                     std::istreambuf_iterator<char>());
}

} // namespace gl

} // namespace esim
