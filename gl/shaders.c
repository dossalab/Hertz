#include <stdlib.h>
#include <stdarg.h>
#include <GL/glew.h>
#include <utils/files.h>
#include <logger/logger.h>

#include "shaders.h"

static void print_shader_compilation_results(GLuint handle)
{
	char *buffer;
	GLint log_size = 0;

	glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &log_size);
	if (log_size == 0) {
		log_i("no compilation logs available...");
		return;
	}

	buffer = calloc(1, log_size);
	if (!buffer) {
		log_e("unable to allocate log buffer - no memory left");
		return;
	}

	glGetShaderInfoLog(handle, log_size, NULL, buffer);

	log_i("shader compilation log: %s", buffer);
	free(buffer);
}

static GLuint compile_single_shader(const char *source, GLenum type)
{
	GLuint handle;
	GLint compilation_status;

	handle = glCreateShader(type);
	if (handle == 0) {
		log_e("unable to create shader!");
		return 0;
	}

	glShaderSource(handle, 1, &source, NULL);

	glCompileShader(handle);
	glGetShaderiv(handle, GL_COMPILE_STATUS, &compilation_status);

	if (compilation_status == GL_FALSE) {
		print_shader_compilation_results(handle);
		log_e("unable to compile shader!");
		glDeleteShader(handle);
		return 0;
	}

	return handle;
}

GLuint load_shader_from_file(const char *filename, GLenum type)
{
	GLuint shader;
	char *source;

	source = read_text_file(filename);
	if (!source) {
		log_e("unable to read shader %s", filename);
		return 0;
	}

	shader = compile_single_shader(source, type);
	free(source);

	return shader;
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

GLuint compile_simple_program(const char *vert_path, const char *frag_path)
{
	GLuint vertex, frag;
	GLuint program = 0;

	vertex = load_shader_from_file(vert_path, GL_VERTEX_SHADER);
	if (!vertex) {
		log_e("unable to load vertex shader (%s)", vert_path);
		return 0;
	}

	frag = load_shader_from_file(frag_path, GL_FRAGMENT_SHADER);
	if (!frag) {
		log_e("unable to load fragment shader (%s)", frag_path);
		glDeleteShader(vertex);
		return 0;
	}

	program = create_shader_program(2, vertex, frag);
	if (!program) {
		log_e("unable to create program!");

		glDeleteShader(vertex);
		glDeleteShader(frag);
		return 0;
	}

	return program;
}
