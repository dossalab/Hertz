#include <stdlib.h>
#include <GL/glew.h>
#include <utils/log.h>
#include <utils/files.h>
#include "io.h"
#include "shaders.h"

GLuint create_shader_from_file(const char *filename, GLenum type)
{
	GLuint shader;
	char *source;
	char *compilation_logs;

	source = read_text_file(filename);
	if (!source) {
		log_e("unable to read shader %s", filename);
		return 0;
	}

	shader = compile_single_shader(source, &compilation_logs, type);
	free(source);

	if (!shader) {
		if (compilation_logs) {
			log_i("shader compilation logs:\n%s", compilation_logs);
			free(compilation_logs);
		}
	}

	return shader;
}

GLuint create_program_from_files(const char *vert_path, const char *frag_path)
{
	GLuint vertex, frag;
	GLuint program = 0;

	vertex = create_shader_from_file(vert_path, GL_VERTEX_SHADER);
	if (!vertex) {
		log_e("unable to load vertex shader (%s)", vert_path);
		return 0;
	}

	frag = create_shader_from_file(frag_path, GL_FRAGMENT_SHADER);
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
