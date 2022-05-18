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
  curl
  GIT_REPOSITORY https://github.com/curl/curl.git
  GIT_TAG        462196e6b4a47f924293a0e26b8e9c23d37ac26f)
FetchContent_MakeAvailable(curl)
add_library(vendor::libcurl ALIAS libcurl)

foreach(MODULE glad stb)

  message(VERBOSE "Configuring ${MODULE}")
  add_subdirectory(${MODULE})
  add_library(vendor::${MODULE} ALIAS ${MODULE})
  message(VERBOSE "Target vendor::${MODULE} added")
  message(VERBOSE "Configuring ${MODULE} -- done")

endforeach()
