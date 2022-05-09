namespace esim {

namespace gl {

inline void program::use() const noexcept {
  assert(id_ != 0);
  glUseProgram(id_);
}

inline GLint program::uniform_location(std::string_view variable) const noexcept {
  assert(id_ != 0);
  return glGetUniformLocation(id_, variable.data());
}

inline GLint program::attribute_location(std::string_view variable) const noexcept {
  assert(id_ != 0);
  return glGetAttribLocation(id_, variable.data());
}

inline program::program() noexcept
    : id_{glCreateProgram()} {}

inline program::~program() noexcept {
  if (id_ != 0) {
    glDeleteProgram(id_);
    id_ = 0;
  }
}

template <typename shader_type, typename... shaders_type>
inline bool program::link_shaders(shader_type &&sdr, shaders_type &&...sdrs) noexcept {
  assert(id_ != 0);
  attach_shader<shader_type, shaders_type...>(std::forward<shader_type>(sdr),
                                              std::forward<shaders_type>(sdrs)...);
  glLinkProgram(id_);
  GLint code; glGetProgramiv(id_, GL_LINK_STATUS, &code);
  if (code == GL_FALSE) {
    GLint maxLength = 0;
    glGetProgramiv(id_, GL_INFO_LOG_LENGTH, &maxLength);
    auto msg = make_uptr<char[]>(maxLength + 1);
    glGetProgramInfoLog(id_, maxLength, &maxLength, msg.get());
    glDeleteProgram(id_);
    id_ = 0;

    std::cout << msg << std::endl;

    return false;
  }

  return false;
}

template <typename curr_sd, typename next_sd, typename... remains>
inline void program::attach_shader(curr_sd &&sd1, next_sd &&sd2, remains &&...sds) noexcept {
  attach_shader_recursive(sd1);
  attach_shader<next_sd, remains...>(std::forward<next_sd>(sd2),
                                     std::forward<remains>(sds)...);
}

template <typename shader_type>
inline void program::attach_shader(shader_type &&sdr) noexcept {
  attach_shader_recursive(sdr);
}

template <typename shader_type>
inline void program::attach_shader_recursive(shader_type &&sdr) noexcept {
  if (!attached_shaders_.count(sdr.id())) {
    glAttachShader(id_, sdr.id());
    
    for (auto &shader : sdr.dependencies()) {
      assert(shader != nullptr);
      attach_shader_recursive(*shader);
    }
  }
}

} // namespace gl

} // namespace esim
