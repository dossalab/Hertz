#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <engine/util.h>
#include <engine/engine.h>

static int prepare_shader(GLenum type, struct region *data)
{
	int success;
	int id;
	const char *src;
	int size;

	src = data->ptr;
	size = data->size;
	id = glCreateShader(type);
	glShaderSource(id, 1, &src, &size);
	glCompileShader(id);

	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (!success) {
		char log[1024];
		glGetShaderInfoLog(id, sizeof(log), NULL, log);
		logger_report("Compilation failed! %s\n", log);

		glDeleteShader(id);
		return -1;
	}

	return id;
}

static int ext_to_type(const char *filename)
{
	char *ext = strrchr(filename, '.');
	if (!ext) {
		return -1;
	}

	ext += 1;

	if (streq(ext, "vert")) {
		return GL_VERTEX_SHADER;
	} else
	if (streq(ext, "frag")) {
		return GL_FRAGMENT_SHADER;
	}

	return -1;
}

int shaders_apply(struct mesh *m, int shader_cnt, ...)
{
	va_list args;
	m->program = glCreateProgram();

	va_start(args, shader_cnt);
	for (int i = 0; i < shader_cnt; i++) {
		char *path;
		int type, id;
		struct region source;

		path = va_arg(args, char *);
		type = ext_to_type(path);
		if (type < 0) {
			logger_report("Unknown shader type, weird...\n");
			return -1;
		}

		if (read_path(path, &source)) {
			logger_report("Unable to open shader!\n");
			return -1;
		}

		id = prepare_shader(type, &source);
		if (id < 0) {
			logger_report("Unable to prepare shader '%s'!\n", path);
			free(source.ptr);
			return -1;
		}

		glAttachShader(m->program, id);
		free(source.ptr);
	}
	va_end (args);

	glLinkProgram(m->program);
	return 0;
}

