add_library(
  ${PROJECT_NAME}_main
  STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/esim_controller.cc)

target_include_directories(
  ${PROJECT_NAME}_main
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(
  ${PROJECT_NAME}_main
  PUBLIC ${PROJECT_NAME}::core
         vendor::glad
         vendor::glm)
