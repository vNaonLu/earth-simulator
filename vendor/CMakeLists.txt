include(FetchContent)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        bf71a834948186f4097caa076cd2663c69a10e1e)
FetchContent_MakeAvailable(glm)
add_library(vendor::glm ALIAS glm)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        45ce5ddd197d5c58f50fdd3296a5131c894e5527)
FetchContent_MakeAvailable(glfw)
add_library(vendor::glfw ALIAS glfw)

FetchContent_Declare(
  httplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
  GIT_TAG        9452c0a4b69c5e4e31169ed32e961d330695122c)
FetchContent_MakeAvailable(httplib)
add_library(vendor::httplib ALIAS httplib)

foreach(MODULE glad stb)

  message(VERBOSE "Configuring ${MODULE}")
  add_subdirectory(${MODULE})
  add_library(vendor::${MODULE} ALIAS ${MODULE})
  message(VERBOSE "Target vendor::${MODULE} added")
  message(VERBOSE "Configuring ${MODULE} -- done")

endforeach()
