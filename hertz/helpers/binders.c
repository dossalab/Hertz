#include HZ_GL_EXTENSIONS_HEADER
#include <hz/helpers/shaders.h>
#include <hz/logger.h>
#include <hz/internal/helpers/binders.h>

static const char *tag = "binder";

static bool bind_single_buffer(struct hz_buffer_binding *b, GLuint program)
{
	GLuint handle;

	if (b->name) {
		handle = hz_create_shader_attribute_buffer(program,
			b->name, b->dimension, b->data, b->size);
	} else {
		handle = hz_create_gl_buffer(b->data,
				sizeof(float) * b->dimension * b->size);
	}

	if (!handle) {
		return false;
	}

	*b->handle = handle;
	return true;
}

static void bind_buffers_unwind(struct hz_buffer_binding *bindings, size_t far)
{
	struct hz_buffer_binding *b;

	for (size_t i = 0; i < far; i++) {
		b = &bindings[i];
		glDeleteBuffers(1, b->handle);
	}
}

bool hz_bind_buffers(struct hz_buffer_binding *bindings, GLuint program,
		size_t len)
{
	struct hz_buffer_binding *b;
	bool ok;

	for (size_t i = 0; i < len; i++) {
		b = &bindings[i];

		ok = bind_single_buffer(b, program);
		if (!ok) {
			hz_log_e(tag, "unable to bind '%s' buffer", b->name);
			bind_buffers_unwind(bindings, i);
			return false;
		}
	}

	return true;
}

bool hz_bind_uniforms(struct hz_uniform_binding *bindings, GLuint program,
		size_t len)
{
	struct hz_uniform_binding *b;

	for (size_t i = 0; i < len; i++) {
		b = &bindings[i];

		*b->handle = glGetUniformLocation(program, b->name);
		if (*b->handle < 0) {
			hz_log_e(tag, "unable to find '%s' uniform", b->name);
			return false;
		}
	}

	return true;
}

