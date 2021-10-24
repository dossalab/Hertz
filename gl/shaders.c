#include <stdlib.h>
#include <stdarg.h>
#include <GL/glew.h>

#include "shaders.h"

static char *get_shader_compilation_results(GLuint handle)
{
	char *buffer;
	GLint log_size = 0;

	glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &log_size);
	if (log_size == 0) {
		return NULL;
	}

	buffer = calloc(1, log_size);
	if (!buffer) {
		return NULL;
	}

	glGetShaderInfoLog(handle, log_size, NULL, buffer);
	return buffer;
}

GLuint compile_single_shader(const char *source, char **logs, GLenum type)
{
	GLuint handle;
	GLint compilation_status;

	handle = glCreateShader(type);
	if (handle == 0) {
		return 0;
	}

	glShaderSource(handle, 1, &source, NULL);

	glCompileShader(handle);
	glGetShaderiv(handle, GL_COMPILE_STATUS, &compilation_status);

	if (compilation_status == GL_FALSE) {
		*logs = get_shader_compilation_results(handle);
		glDeleteShader(handle);
		return 0;
	}

	return handle;
}

GLuint create_shader_program(size_t shader_count, ...)
{
	GLuint program;
	GLuint shader;
	va_list args;

	program = glCreateProgram();
	if (!program) {
		return 0;
	}

	va_start(args, shader_count);

	for (size_t i = 0; i < shader_count; i++) {
		shader = va_arg(args, GLuint);
		glAttachShader(program, shader);
	}

	va_end(args);

	glLinkProgram(program);
	return program;
}
