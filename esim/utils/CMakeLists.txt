add_library(
  ${PROJECT_NAME}_utils
  INTERFACE)

target_include_directories(
  ${PROJECT_NAME}_utils
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
  ${PROJECT_NAME}_utils
  INTERFACE ${PROJECT_NAME}::trans
            vendor::glad
            vendor::glfw
            vendor::stb)