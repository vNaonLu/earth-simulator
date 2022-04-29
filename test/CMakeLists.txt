include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        e2239ee6043f73722e7aa812a459f54a28552929)
FetchContent_MakeAvailable(googletest)

add_executable(
  ${PROJECT_NAME}_test 
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/coord.cc)

target_include_directories(
  ${PROJECT_NAME}_test
  PRIVATE ${CMAKE_SOURCE_DIR}/src
          ${CMAKE_SOURCE_DIR}/vendor
          ${CMAKE_SOURCE_DIR}/vendor/glfw/include
          ${CMAKE_SOURCE_DIR}/vendor/glm)

target_link_libraries(
  ${PROJECT_NAME}_test
  PRIVATE gtest
          VENDOR::glad
          VENDOR::glfw
          VENDOR::glm)