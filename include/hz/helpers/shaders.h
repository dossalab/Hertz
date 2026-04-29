#pragma once

#include HZ_GL_HEADER
#include <stddef.h>

GLuint hz_create_gl_buffer(void *data, size_t len);
GLuint hz_create_shader_attribute_buffer(GLuint shader, const char *name,
		size_t components, void *data, size_t len);
