set(ESIM_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(
  ${PROJECT_NAME}_main
  STATIC ${ESIM_SOURCE_DIR}/esim_controller.cc
         ${ESIM_SOURCE_DIR}/esim_controller_opaque.cc
         ${ESIM_SOURCE_DIR}/esim_engine.cc
         ${ESIM_SOURCE_DIR}/esim_engine_opaque.cc
         ${ESIM_SOURCE_DIR}/esim_render_pipe.cc
         ${ESIM_SOURCE_DIR}/details/basemap_storage.cc
         ${ESIM_SOURCE_DIR}/details/surface_vertex_engine.cc
         ${ESIM_SOURCE_DIR}/scene/stellar.cc
         ${ESIM_SOURCE_DIR}/scene/surface_tile.cc
         ${ESIM_SOURCE_DIR}/scene/surface_collections.cc
         ${ESIM_SOURCE_DIR}/scene/atmosphere.cc
         ${ESIM_SOURCE_DIR}/scene/skysphere.cc)

target_include_directories(
  ${PROJECT_NAME}_main
  PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
          ${OPENSSL_INCLUDE_DIR}
  PRIVATE ${ESIM_SOURCE_DIR})

target_link_directories(
  ${PROJECT_NAME}_main
  PUBLIC ${OPENSSL_LIBRARIES_DIR})

target_link_libraries(
  ${PROJECT_NAME}_main
  PUBLIC ${PROJECT_NAME}::core
          vendor::glfw
          vendor::glad
          vendor::glm
          vendor::httplib
          opengl32)
