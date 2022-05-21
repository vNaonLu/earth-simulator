add_library(
  ${PROJECT_NAME}_core
  STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/bitmap.cc
         ${CMAKE_CURRENT_SOURCE_DIR}/src/observer.cc
         ${CMAKE_CURRENT_SOURCE_DIR}/src/publisher.cc)

target_include_directories(
  ${PROJECT_NAME}_core
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(
  ${PROJECT_NAME}_core
  PUBLIC vendor::glm)