cmake_minimum_required(VERSION 3.20)

project(
  esim
  LANGUAGES CXX C
  VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
  if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /wd4201 /wd4819 /wd4324")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11")
endif()

add_subdirectory(vendor)

set(ES_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(
  ${PROJECT_NAME}
  ${ES_SOURCE_DIR}/main.cc
  ${ES_SOURCE_DIR}/scene/layers/surface_node.cc
  ${ES_SOURCE_DIR}/scene/layers/surface_layer.cc
  ${ES_SOURCE_DIR}/scene/scene_controller.cc
  ${ES_SOURCE_DIR}/scene/scene_renderer.cc
  ${ES_SOURCE_DIR}/scene/scene_engine.cc)

target_include_directories(
  ${PROJECT_NAME} 
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
          ${CMAKE_CURRENT_SOURCE_DIR}/vendor
          ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw/include
          ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE opengl32
          VENDOR::glad
          VENDOR::glfw
          VENDOR::glm)

file(GLOB GLSL_SOURCE ${ES_SOURCE_DIR}/glsl/*)
file(COPY ${GLSL_SOURCE} DESTINATION ${CMAKE_BINARY_DIR}/Debug/glsl)

add_subdirectory(test)