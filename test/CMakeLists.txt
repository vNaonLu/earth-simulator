include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        e2239ee6043f73722e7aa812a459f54a28552929)
FetchContent_MakeAvailable(googletest)

add_executable(
  ${PROJECT_NAME}_test 
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/test_subject_observer.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/test_transform.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/test_fifo.cc)

target_link_libraries(
  ${PROJECT_NAME}_test
  PRIVATE gtest
          ${PROJECT_NAME}::core
          vendor::glad
          vendor::glfw
          vendor::glm)