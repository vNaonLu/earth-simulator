add_library(
  ${PROJECT_NAME}_trans
  STATIC ${CMAKE_CURRENT_SOURCE_DIR}/transform/wgs84.cc)

target_include_directories(
  ${PROJECT_NAME}_trans
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
  ${PROJECT_NAME}_trans
  PUBLIC vendor::glm)