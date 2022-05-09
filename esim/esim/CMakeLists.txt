add_library(
  ${PROJECT_NAME}
  STATIC ${CMAKE_CURRENT_SOURCE_DIR}/scene/scene_engine.cc
         ${CMAKE_CURRENT_SOURCE_DIR}/scene/scene_controller.cc
         ${CMAKE_CURRENT_SOURCE_DIR}/scene/scene_renderer.cc
         ${CMAKE_CURRENT_SOURCE_DIR}/scene/layers/debug_layer.cc
         ${CMAKE_CURRENT_SOURCE_DIR}/scene/layers/solar.cc
         ${CMAKE_CURRENT_SOURCE_DIR}/scene/layers/outer_space.cc
         ${CMAKE_CURRENT_SOURCE_DIR}/scene/layers/atmosphere.cc
         ${CMAKE_CURRENT_SOURCE_DIR}/scene/layers/surface_layer.cc
         ${CMAKE_CURRENT_SOURCE_DIR}/scene/layers/surface_node.cc)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC ${PROJECT_NAME}::core
         vendor::glad
         vendor::glm)
