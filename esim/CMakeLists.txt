cmake_minimum_required(VERSION 3.20)

foreach(MODULE core main trans utils scene)

  message(VERBOSE "Configuring ${MODULE}")
  add_subdirectory(${MODULE})
  add_library(${PROJECT_NAME}::${MODULE} ALIAS ${PROJECT_NAME}_${MODULE})
  message(VERBOSE "Target ${PROJECT_NAME}::${MODULE} added")
  message(VERBOSE "Configuring ${MODULE} -- done")

endforeach()

add_executable(
  ${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}/esim_main.cc)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE ${PROJECT_NAME}::core
          ${PROJECT_NAME}::utils
          ${PROJECT_NAME}::trans
          ${PROJECT_NAME}::scene
          vendor::glfw
          vendor::glad
          vendor::glm
          opengl32)

# file(GLOB ${ESIM_ASSETS} ${CMAKE_CURRENT_SOURCE_DIR}/assets)
# file(COPY ${ESIM_ASSETS} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)
message(VERBOSE "move ${CMAKE_CURRENT_SOURCE_DIR}/assets to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets")


add_executable(
  ${PROJECT_NAME}_r
  ${CMAKE_CURRENT_SOURCE_DIR}/esim_refactor.cc)

target_link_libraries(
  ${PROJECT_NAME}_r
  PRIVATE ${PROJECT_NAME}::main)