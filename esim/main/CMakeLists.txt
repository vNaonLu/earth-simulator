set(ESIM_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(
  ${PROJECT_NAME}_main
  STATIC ${ESIM_SOURCE_DIR}/esim_controller.cc
         ${ESIM_SOURCE_DIR}/esim_controller_opaque.cc
         ${ESIM_SOURCE_DIR}/esim_engine.cc
         ${ESIM_SOURCE_DIR}/esim_engine_opaque.cc
         ${ESIM_SOURCE_DIR}/scene/impl/surface_tile.cc
         ${ESIM_SOURCE_DIR}/scene/impl/surface_collections.cc)

target_include_directories(
  ${PROJECT_NAME}_main
  PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE ${ESIM_SOURCE_DIR})

target_link_libraries(
  ${PROJECT_NAME}_main
  PUBLIC ${PROJECT_NAME}::core
          vendor::glfw
          vendor::glad
          vendor::glm
          opengl32)
