#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <scene/scene_controller.h>
#include <scene/scene_engine.h>
#include <utils/keys.h>

static void error_callback(int, const char *);
static void key_callback(GLFWwindow *, int, int, int, int);
static void scroll_callback(GLFWwindow *, double, double);
static void framebuffer_size_callback(GLFWwindow *, int, int);
static void window_refresh_callback(GLFWwindow *);

esim::u_ptr<esim::scene::scene_engine>     esim_engine;
esim::u_ptr<esim::scene::scene_controller> esim_ctrler;

int main(...) {
  GLFWwindow* window;
  glfwSetErrorCallback(error_callback);

  if (!glfwInit()) {
    std::cout << "[x] failed to initialize glfw." << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  int width = 1080,
      height = 1080;
  window = glfwCreateWindow(width, height, "Earth Simulator", NULL, NULL);
  if (!window) {
    std::cout << "[x] failed to create window." << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetWindowSizeLimits(window, 600, 600, GLFW_DONT_CARE, GLFW_DONT_CARE);
  glfwSetKeyCallback(window, key_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetWindowRefreshCallback(window, window_refresh_callback);

  glfwMakeContextCurrent(window);
  gladLoadGL();
  glfwSwapInterval(1);
  glfwGetFramebufferSize(window, &width, &height);

  esim_engine = esim::make_ptr_u<esim::scene::scene_engine>();
  esim_ctrler = esim::make_ptr_u<esim::scene::scene_controller>();
  esim_ctrler->subscribe(esim_engine.get());
  esim_engine->subscribe(esim_ctrler.get());
  esim_ctrler->update_viewport(width, height);
  esim_engine->start(
      [&]() {
        glfwPollEvents();
        if (glfwWindowShouldClose(window)) {
          esim_engine->stop();
        }
      },
      [&]() { glfwSwapBuffers(window); });

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}

void window_refresh_callback(GLFWwindow *window) {
  esim_engine->render();
  glfwSwapBuffers(window);
}

static void error_callback([[maybe_unused]] int error, const char *msg) {
  std::cout << "[x] error occured: " << msg << std::endl;
}

static void scroll_callback([[maybe_unused]] GLFWwindow *window,
                            [[maybe_unused]] double xoffset,
                            [[maybe_unused]] double yoffset) {
  esim_ctrler->zooming(yoffset);
}

void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window,
                               [[maybe_unused]] int width,
                               [[maybe_unused]] int height) {
  esim_ctrler->update_viewport(width, height);
}

static void key_callback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, [[maybe_unused]] int action, [[maybe_unused]] int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
    esim_ctrler->stop();
  } else {
    if (auto keycode = esim::utils::glfw_to_keycode(key); keycode != -1) {
      if (action == GLFW_PRESS) {
        esim_ctrler->press(keycode);
      } else if (action == GLFW_RELEASE) {
        esim_ctrler->release(keycode);
      }
    }
  }
}