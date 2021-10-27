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

void use_shader_program(GLuint shader)
{
	glUseProgram(shader);
}

GLint get_shader_attribute_handle(GLuint shader, const char *name)
{
	return glGetAttribLocation(shader, name);
}

GLint get_shader_uniform_handle(GLuint shader, const char *name)
{
	return glGetUniformLocation(shader, name);
}

void set_shader_uniform_matrix(GLint uniform, mat4x4 value)
{
	glUniformMatrix4fv(uniform, 1, GL_FALSE, (float *)value);
}

void set_shader_uniform_float(GLint uniform, float value)
{
	glUniform1f(uniform, value);
}

GLuint create_shader_attribute_buffer(GLuint shader, const char *name,
		size_t components, void *data, size_t len)
{
	GLuint buffer;
	GLint attribute;
	const GLenum data_type = GL_FLOAT;
	const size_t byte_len = sizeof(float) * len * components;

	attribute = get_shader_attribute_handle(shader, name);
	if (attribute < 0) {
		return 0;
	}

	glGenBuffers(1, &buffer);
	if (!buffer) {
		return 0;
	}

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, byte_len, data, GL_STATIC_DRAW);

	glVertexAttribPointer(attribute, components, data_type, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(attribute);

	return buffer;
}

void delete_shader_attribute_buffer(GLuint buffer)
{
	glDeleteBuffers(1, &buffer);
}
