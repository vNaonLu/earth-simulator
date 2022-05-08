#ifndef __ESIM_CORE_GLAPI_DEBUG_H_
#define __ESIM_CORE_GLAPI_DEBUG_H_

#include <cassert>
#include <glad/glad.h>
#include <iostream>

#define ESIM_GL_TRACE()                                                \
  {                                                                    \
    GLenum err_code = glGetError();                                    \
    if (err_code != GL_NO_ERROR) {                                     \
      std::cout << "[x] get GL error ";                                \
      switch (err_code) {                                              \
      case GL_INVALID_ENUM:                                            \
        std::cout << "GL_INVALID_ENUM";                                \
        break;                                                         \
      case GL_INVALID_VALUE:                                           \
        std::cout << "GL_INVALID_VALUE";                               \
        break;                                                         \
      case GL_INVALID_OPERATION:                                       \
        std::cout << "GL_INVALID_OPERATION";                           \
        break;                                                         \
      case GL_INVALID_FRAMEBUFFER_OPERATION:                           \
        std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION";               \
        break;                                                         \
      case GL_OUT_OF_MEMORY:                                           \
        std::cout << "GL_OUT_OF_MEMORY";                               \
        break;                                                         \
      case GL_STACK_UNDERFLOW:                                         \
        std::cout << "GL_STACK_UNDERFLOW";                             \
        break;                                                         \
      case GL_STACK_OVERFLOW:                                          \
        std::cout << "GL_STACK_OVERFLOW";                              \
        break;                                                         \
                                                                       \
      default:                                                         \
        break;                                                         \
      }                                                                \
      std::cout << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
      /*assert(false);*/                                               \
    }                                                                  \
  }

#define ESIM_GL_TRACE_FLUSH()                                        \
  {                                                                  \
    size_t error_got = 0;                                            \
    while (glGetError()) {                                           \
      ++error_got;                                                   \
    }                                                                \
    std::cout << "[x] total flush GL " << error_got << " error(s) "; \
    std::cout << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
  }

#endif