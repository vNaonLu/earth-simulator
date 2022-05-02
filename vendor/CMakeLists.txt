foreach(MODULE glad glfw glm)

  message(VERBOSE "Configuring ${MODULE}")
  add_subdirectory(${MODULE})
  add_library(vendor::${MODULE} ALIAS ${MODULE})
  message(VERBOSE "Target vendor::${MODULE} added")
  message(VERBOSE "Configuring ${MODULE} -- done")

endforeach()
