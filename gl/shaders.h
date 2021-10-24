#ifndef GL_SHADERS_H
#define GL_SHADERS_H

#include <GL/gl.h>
#include <stddef.h>

GLuint compile_single_shader(const char *source, char **logs, GLenum type);
GLuint create_shader_program(size_t shader_count, ...);

#endif
