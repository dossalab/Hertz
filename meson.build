project('libhertz', 'c',
  version : '1.0.0',
  license : 'LGPL')
soversion = '0'

cc = meson.get_compiler('c')

add_global_arguments('-DHZ_GL_HEADER=<GL/gl.h>', language : 'c')
add_global_arguments('-DHZ_GL_EXTENSIONS_HEADER=<GL/glew.h>', language : 'c')

assimp_dep = dependency('assimp')

hertz_dep = []
hertz_dep += cc.find_library('m')
hertz_dep += dependency('glfw3')
hertz_dep += dependency('glew')
hertz_dep += dependency('opengl')

hertz_inc = include_directories('include')
hertz_src = [
  'hertz/material.c',
  'hertz/node.c',
  'hertz/nodes/camera.c',
  'hertz/nodes/mesh.c',
  'hertz/nodes/light.c',
  'hertz/nodes/root.c',
  'hertz/actors/fly.c',
  'hertz/util/logger.c',
  'hertz/util/arena.c',
  'hertz/contexts/glfw.c',
  'hertz/context.c',
  'hertz/built-in/shaders/simple.c',
  'hertz/helpers/shaders.c',
  'hertz/helpers/binders.c',
  'hertz/helpers/textures.c',
  'hertz/adt/list.c',
  'hertz/adt/tree.c'
]

libhertz = library('hertz', hertz_src,
  version : meson.project_version(),
  soversion : soversion,
  include_directories : hertz_inc,
  dependencies : hertz_dep,
  install : true)

executable('01-assimp',
  [ 'examples/01-assimp/assimp.c',
    'examples/01-assimp/main.c',
    'examples/utils/stb_image.c' ],
  include_directories : hertz_inc,
  dependencies : [ hertz_dep, assimp_dep ],
  link_with : libhertz)
