foreach(MODULE glad glfw glm)

  message(VERBOSE "Configuring ${MODULE}")
  add_subdirectory(${MODULE})
  add_library(VENDOR::${MODULE} ALIAS ${MODULE})
  message(VERBOSE "Target VENDOR::${MODULE} added")
  message(VERBOSE "Configuring ${MODULE} -- done")

endforeach()
