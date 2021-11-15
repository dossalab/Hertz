#ifndef GL_SHADERS_H
#define GL_SHADERS_H

#include <GL/gl.h>
#include <stddef.h>
#include <utils/linmath.h>

GLuint compile_single_shader(const char *source, char **logs, GLenum type);
GLuint create_shader_program(size_t shader_count, ...);
void use_shader_program(GLuint shader);

GLint get_shader_attribute_handle(GLuint shader, const char *name);
GLint get_shader_uniform_handle(GLuint shader, const char *name);
void set_shader_uniform_matrix(GLint uniform, mat4x4 value);
void set_shader_uniform_float(GLint uniform, float value);

GLuint create_shader_attribute_buffer(GLuint shader, const char *name,
		size_t components, void *data, size_t len);

void delete_shader_attribute_buffer(GLuint buffer);

#endif
