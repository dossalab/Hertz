#pragma once

#include HZ_GL_HEADER

GLuint hz_compile_single_shader(const char *source, char **logs, GLenum type);
GLuint hz_create_program(size_t shader_count, ...);
GLuint hz_create_program_from_source(const char *vert_src,
		const char *frag_src, char **logs);
