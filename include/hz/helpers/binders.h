#ifndef HZ_HELPERS_BINDERS_H
#define HZ_HELPERS_BINDERS_H

#include HZ_GL_HEADER
#include <stddef.h>
#include <stdbool.h>
#include <hz/utils/common.h>

struct hz_buffer_binding {
	GLuint *handle;
	const char *name;
	unsigned dimension;
	void *data;
	size_t size;
};

struct hz_uniform_binding {
	GLint *handle;
	const char *name;
};

bool hz_bind_buffers(struct hz_buffer_binding *bindings, GLuint program,
	size_t len);
bool hz_bind_uniforms(struct hz_uniform_binding *bindings, GLuint program,
	size_t len);

#endif
