#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

#include "camera.h"
#include "coord/transform.h"
#include "surface_node.h"

static void error_callback(int error, const char *msg);
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

extern int idx;
  
int main(...) {
  int width = 1080, height = 1080;
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
  glfwMakeContextCurrent(window);
  gladLoadGL();
  glfwSwapInterval(1);

  esim::surface_node p_surf1(0, 0, 0, [](const char *msg) {
    std::cout << "[x] error: " << msg << std::endl;
  });

  esim::camera camera{width, height, glm::vec3(0.f, 0.f, 3.f)};

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LEQUAL);
    glDepthFunc(GL_ALWAYS);
    glFrontFace(GL_CCW);
    glfwGetFramebufferSize(window, &width, &height);
    camera.set_viewport(width, height);
    glViewport(0, 0, width, height);

    p_surf1.draw(camera);

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

static void key_callback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, [[maybe_unused]] int action, [[maybe_unused]] int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  } else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
    ++idx;
  } else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
    --idx;
  } 
}