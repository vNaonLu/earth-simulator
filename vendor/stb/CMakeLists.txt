add_library(
  stb 
  STATIC ${CMAKE_CURRENT_SOURCE_DIR}/stb/deprecated/stb_image.c)

target_include_directories(
  stb
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/stb)