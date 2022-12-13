#pragma once

#include HZ_GL_HEADER
#include <stddef.h>

/* FIXME - for 'free' */
#include <stdlib.h>

GLuint hz_compile_single_shader(const char *source, char **logs, GLenum type);
GLuint hz_create_program(size_t shader_count, ...);
GLuint hz_create_program_from_source(const char *vert_src,
		const char *frag_src, char **logs);

GLuint hz_create_gl_buffer(void *data, size_t len);
GLuint hz_create_shader_attribute_buffer(GLuint shader, const char *name,
		size_t components, void *data, size_t len);
