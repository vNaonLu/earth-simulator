add_library(
  ${PROJECT_NAME}_scene
  STATIC ${CMAKE_CURRENT_SOURCE_DIR}/scene/scene_engine.cc
         ${CMAKE_CURRENT_SOURCE_DIR}/scene/scene_controller.cc
         ${CMAKE_CURRENT_SOURCE_DIR}/scene/scene_renderer.cc
         ${CMAKE_CURRENT_SOURCE_DIR}/scene/layers/debug_layer.cc
         ${CMAKE_CURRENT_SOURCE_DIR}/scene/layers/surface_layer.cc
         ${CMAKE_CURRENT_SOURCE_DIR}/scene/layers/surface_node.cc)

target_include_directories(
  ${PROJECT_NAME}_scene
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
  ${PROJECT_NAME}_scene
  PUBLIC ${PROJECT_NAME}::trans
         ${PROJECT_NAME}::utils
         vendor::glad
         vendor::glm)
