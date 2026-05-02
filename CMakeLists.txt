cmake_minimum_required(VERSION 3.20)
project(libhertz VERSION 1.0.0 LANGUAGES C)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)

# GLAD2: FetchContent downloads glad's CMake module, which calls the installed
# `glad` generator to produce the OpenGL loader at configure time.
include(FetchContent)
FetchContent_Declare(glad2
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG        v2.0.8
  GIT_SUBMODULES ""
  SOURCE_SUBDIR  cmake
)
FetchContent_MakeAvailable(glad2)
glad_add_library(glad REPRODUCIBLE EXCLUDE_FROM_ALL API gl:core=3.3)

add_library(hertz SHARED
  hertz/material.c
  hertz/program.c
  hertz/node.c
  hertz/nodes/camera.c
  hertz/nodes/mesh.c
  hertz/nodes/light.c
  hertz/nodes/root.c
  hertz/actors/fly.c
  hertz/actors/orbit.c
  hertz/util/logger.c
  hertz/util/arena.c
  hertz/util/panic.c
  hertz/contexts/glfw.c
  hertz/context.c
  hertz/built-in/shaders/phong.c
  hertz/built-in/shaders/wireframe.c
  hertz/built-in/materials/checkerboard.c
  hertz/built-in/materials/solid.c
  hertz/built-in/primitives/cube.c
  hertz/built-in/primitives/plane.c
  hertz/built-in/primitives/sphere.c
  hertz/helpers/shaders.c
  hertz/helpers/binders.c
  hertz/helpers/textures.c
  hertz/adt/list.c
  hertz/adt/map.c
  hertz/adt/tree.c
  hertz/material_store.c
)

set_target_properties(hertz PROPERTIES
  VERSION   ${PROJECT_VERSION}
  SOVERSION 0
)

target_include_directories(hertz
  PUBLIC  include
  PRIVATE ${CMAKE_SOURCE_DIR}
)

target_compile_options(hertz PUBLIC
  -DHZ_GL_HEADER=<glad/gl.h>
  -DHZ_GL_EXTENSIONS_HEADER=<glad/gl.h>
)

target_link_libraries(hertz PUBLIC m glfw glad OpenGL::GL)

install(TARGETS hertz)
install(DIRECTORY include/hz TYPE INCLUDE)

# Examples
add_executable(assimp_example
  examples/assimp/assimp.c
  examples/assimp/main.c
  examples/utils/stb_image.c
)
target_include_directories(assimp_example PRIVATE include ${CMAKE_SOURCE_DIR})
target_link_libraries(assimp_example PRIVATE hertz assimp::assimp)

add_executable(primitives
  examples/primitives/primitives.c
)
target_include_directories(primitives PRIVATE include)
target_link_libraries(primitives PRIVATE hertz)
