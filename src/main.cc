#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

#include "camera.h"
#include "coord/transform.h"
#include <controller/mouse.h>
#include <glapi/gl_helper.h>
#include <scene/surface_layer.h>

static void error_callback(int, const char *);
static void key_callback(GLFWwindow *, int, int, int, int);
static void scroll_callback(GLFWwindow *, double, double);
int width = 1080,
    height = 1080;
esim::camera camera{width, height, glm::vec3{0.f, 0.f, 3.f}, glm::vec3{0.f, 0.f, 1.f}};

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
  window = glfwCreateWindow(width, height, "Simple example", NULL, NULL);
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

  esim::scene::surface_layer base_layer(15);

  while (!glfwWindowShouldClose(window)) {
    glfwGetFramebufferSize(window, &width, &height);
    camera.set_viewport(width, height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // glDepthFunc(GL_ALWAYS);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    base_layer.draw(camera);
    base_layer.draw_bounding_box(camera);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
 
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
    esim::ctrl::mouse::vertical_zoom_in(camera, -yoffset);
  } else {
    esim::ctrl::mouse::vertical_zoom_out(camera, yoffset);
  }
}

static void key_callback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, [[maybe_unused]] int action, [[maybe_unused]] int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}