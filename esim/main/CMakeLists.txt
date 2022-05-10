add_library(
  ${PROJECT_NAME}_main
  STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/esim_controller.cc
         ${CMAKE_CURRENT_SOURCE_DIR}/src/esim_engine.cc)

target_include_directories(
  ${PROJECT_NAME}_main
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(
  ${PROJECT_NAME}_main
  PUBLIC ${PROJECT_NAME}::core
          vendor::glfw
          vendor::glad
          vendor::glm
          opengl32)
