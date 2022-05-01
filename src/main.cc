#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <scene/scene_engine.h>

static void error_callback(int, const char *);
static void key_callback(GLFWwindow *, int, int, int, int);
static void scroll_callback(GLFWwindow *, double, double);
int width = 1080,
    height = 1080;

esim::u_ptr<esim::scene::scene_engine> esim_engine;

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
  window = glfwCreateWindow(width, height, "Earth Simulator", NULL, NULL);
  if (!window) {
    std::cout << "[x] failed to create window." << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetKeyCallback(window, key_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwMakeContextCurrent(window);
  gladLoadGL();
  glfwSwapInterval(1);

  esim_engine = esim::make_ptr_u<esim::scene::scene_engine>();

  esim_engine->start(
      [&]() {
        if (!glfwWindowShouldClose(window)) {
          glfwGetFramebufferSize(window, &width, &height);
          esim_engine->update_viewport(width, height);
        } else {
          esim_engine->stop();
        }
      },
      [&]() {
        glfwSwapBuffers(window);
        glfwPollEvents();
      });
 
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}

static void error_callback([[maybe_unused]] int error, const char *msg) {
  std::cout << "[x] error occured: " << msg << std::endl;
}

static void scroll_callback([[maybe_unused]] GLFWwindow *window,
                            [[maybe_unused]] double xoffset,
                            [[maybe_unused]] double yoffset) {
  if (yoffset < 0) {
    esim_engine->zoom_in(-yoffset);
  } else {
    esim_engine->zoom_out(yoffset);
  }
}

static void key_callback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, [[maybe_unused]] int action, [[maybe_unused]] int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}