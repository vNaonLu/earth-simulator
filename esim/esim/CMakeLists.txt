add_library(
  ${PROJECT_NAME}
  STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/esim_controller.cc)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC ${PROJECT_NAME}::core
         vendor::glad
         vendor::glm)
