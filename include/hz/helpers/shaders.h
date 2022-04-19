#ifndef UTILS_GL_SHADERS_H
#define UTILS_GL_SHADERS_H

#include <GL/gl.h>
#include <stddef.h>

GLuint hz_compile_single_shader(const char *source, char **logs, GLenum type);
GLuint hz_create_shader_program(size_t shader_count, ...);

GLuint hz_create_gl_buffer(void *data, size_t len);
GLuint hz_create_shader_attribute_buffer(GLuint shader, const char *name,
		size_t components, void *data, size_t len);

void hz_delete_gl_buffer(GLuint buffer);

#endif
