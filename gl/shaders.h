#ifndef GL_SHADERS_H
#define GL_SHADERS_H

#include <GL/gl.h>
#include <stddef.h>

GLuint load_shader_from_file(const char *filename, GLenum type);
GLuint create_shader_program(size_t shader_count, ...);
GLuint complile_simple_program(const char *vert_path, const char *frag_path);

#endif
