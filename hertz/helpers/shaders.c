#include <stdlib.h>
#include <stdarg.h>
#include HZ_GL_EXTENSIONS_HEADER

#include <hz/helpers/shaders.h>

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

GLuint hz_compile_single_shader(const char *source, char **logs, GLenum type)
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

GLuint hz_create_program(size_t shader_count, ...)
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

GLuint hz_create_program_from_source(const char *vert_src,
		const char *frag_src, char **logs)
{
	GLuint vertex, frag;
	GLuint program;

	vertex = hz_compile_single_shader(vert_src, logs, GL_VERTEX_SHADER);
	if (!vertex) {
		goto fail;
	}

	frag = hz_compile_single_shader(frag_src, logs, GL_FRAGMENT_SHADER);
	if (!frag) {
		goto fail_delete_vertex_shader;
	}

	program = hz_create_program(2, vertex, frag);
	if (!program) {
		goto fail_delete_all_shaders;
	}

	return program;

fail_delete_all_shaders:
	glDeleteShader(frag);
fail_delete_vertex_shader:
	glDeleteShader(vertex);
fail:
	return 0;
}

GLuint hz_create_gl_buffer(void *data, size_t len)
{
	GLuint buffer;

	glGenBuffers(1, &buffer);
	if (!buffer) {
		return 0;
	}

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, len, data, GL_STATIC_DRAW);

	return buffer;
}

GLuint hz_create_shader_attribute_buffer(GLuint shader, const char *name,
		size_t components, void *data, size_t len)
{
	GLuint buffer;
	GLint attribute;
	const GLenum data_type = GL_FLOAT;
	const size_t byte_len = sizeof(float) * len * components;

	attribute = glGetAttribLocation(shader, name);
	if (attribute < 0) {
		return 0;
	}

	buffer = hz_create_gl_buffer(data, byte_len);
	if (!buffer) {
		return 0;
	}

	glVertexAttribPointer(attribute, components, data_type, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(attribute);

	return buffer;
}
