#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <esim/esim_controller.h>
#include <glad/glad.h>
#include <iostream>

static void error_callback(int, const char *);
static void key_callback(GLFWwindow *, int, int, int, int);
static void scroll_callback(GLFWwindow *, double, double);
static void framebuffer_size_callback(GLFWwindow *, int, int);
static void window_refresh_callback(GLFWwindow *);
static void cursor_position_callback(GLFWwindow *, double, double);
static void mouse_button_callback(GLFWwindow *, int, int, int);
inline esim::protocol::keycode_type glfw_to_keycode(int) noexcept;

esim::uptr<esim::esim_controller> esim_ctrler;

int main(...) {
  GLFWwindow* window;
  glfwSetErrorCallback(error_callback);

  if (!glfwInit()) {
    std::cerr << "[x] failed to initialize glfw." << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  int width = 1080, height = 1080;
  window = glfwCreateWindow(width, height, "Earth Simulator", NULL, NULL);
  if (!window) {
    std::cerr << "[x] failed to create window." << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetWindowSizeLimits(window, 600, 600, GLFW_DONT_CARE, GLFW_DONT_CARE);
  glfwSetKeyCallback(window, key_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetWindowRefreshCallback(window, window_refresh_callback);
  glfwSetCursorPosCallback(window, cursor_position_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);

  glfwMakeContextCurrent(window);
  gladLoadGL();
  glfwSwapInterval(1);
  glfwGetFramebufferSize(window, &width, &height);
  esim_ctrler = esim::make_uptr<esim::esim_controller>();
  esim_ctrler->update_viewport(width, height);
  esim_ctrler->bind_before_render_process([&]() {
    glfwPollEvents();
    if (glfwWindowShouldClose(window)) {
      esim_ctrler->stop();
    }
  });
  esim_ctrler->bind_after_render_process([&]() { 
    glfwSwapBuffers(window);
  });
  esim_ctrler->start();

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}

void window_refresh_callback(GLFWwindow *window) {
  if (nullptr != esim_ctrler) {
    esim_ctrler->render();
    glfwSwapBuffers(window);
  }
}

static void error_callback([[maybe_unused]] int error, const char *msg) {
  std::cerr << "[x] error occured: " << msg << std::endl;
}

static void scroll_callback([[maybe_unused]] GLFWwindow *window,
                            [[maybe_unused]] double xoffset,
                            [[maybe_unused]] double yoffset) {
}

void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window,
                               [[maybe_unused]] int width,
                               [[maybe_unused]] int height) {
  if (nullptr != esim_ctrler) {
    esim_ctrler->update_viewport(width, height);
  }
}

static void key_callback(GLFWwindow *window, int key, [[maybe_unused]] int scancode,
                         [[maybe_unused]] int action, [[maybe_unused]] int mods) {
  if (nullptr != esim_ctrler) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
      esim_ctrler->stop();
    } else {
      if (auto keycode = glfw_to_keycode(key); keycode != -1) {
        if (action == GLFW_PRESS) {
          esim_ctrler->key_press(keycode);
        } else if (action == GLFW_RELEASE) {
          esim_ctrler->key_release(keycode);
        }
      }
    }
  }
}

static void cursor_position_callback([[maybe_unused]] GLFWwindow *window, double xpos, double ypos) {
  if (nullptr != esim_ctrler) {
    esim_ctrler->mouse_move(xpos, ypos);
  }
}

void mouse_button_callback([[maybe_unused]] GLFWwindow *window,
                           int button, int action,
                           [[maybe_unused]] int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    if (action == GLFW_PRESS) {
      esim_ctrler->left_mouse_press();
    } else if (action == GLFW_RELEASE) {
      esim_ctrler->left_mouse_release();
    }
  }
}

inline esim::protocol::keycode_type glfw_to_keycode(int glfw_key) noexcept {
  switch(glfw_key) {
  case GLFW_KEY_LEFT_CONTROL:
    [[fallthrough]];
  case GLFW_KEY_RIGHT_CONTROL:
    return esim::protocol::KEY_CTRL;

  case GLFW_KEY_PAGE_UP:
    return esim::protocol::KEY_PAGEUP;
  case GLFW_KEY_PAGE_DOWN:
    return esim::protocol::KEY_PAGEDOWN;

  case GLFW_KEY_LEFT:
    return esim::protocol::KEY_LEFT;
  case GLFW_KEY_UP:
    return esim::protocol::KEY_UP;
  case GLFW_KEY_RIGHT:
    return esim::protocol::KEY_RIGHT;
  case GLFW_KEY_DOWN:
    return esim::protocol::KEY_DOWN;

  case GLFW_KEY_1:
    return esim::protocol::KEY_ONE;
  case GLFW_KEY_2:
    return esim::protocol::KEY_TWO;
  case GLFW_KEY_3:
    return esim::protocol::KEY_THREE;

  case GLFW_KEY_B:
    return esim::protocol::KEY_B;

  default:
    return esim::protocol::KEY_NONE;
  }
}