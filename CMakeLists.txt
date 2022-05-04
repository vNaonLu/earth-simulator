cmake_minimum_required(VERSION 3.20)

project(
  esim
  LANGUAGES CXX C
  VERSION 0.1.0)

set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS  OFF)
set(GLFW_BUILD_DOCS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/debug)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release)

set(CMAKE_CXX_STANDARD 17)
  if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /wd4201 /wd4819 /wd4324")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11")
endif()

foreach(MODULE vendor esim test)

  message(VERBOSE "Configuring ${MODULE}")
  add_subdirectory(${MODULE})
  message(VERBOSE "Configuring ${MODULE} -- done")

endforeach()