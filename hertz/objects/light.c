#include GL_EXTENSIONS_HEADER
#include <hz/objects/light.h>
#include <hz/logger.h>
#include <vendor/linmath/linmath.h>
#include <stdio.h>
#include <stdarg.h>

static const char *tag = "light";

static inline GLint glGetUniformLocationf(GLuint program, const char *fmt, ...) {
	char str[128];
	va_list list;

	va_start(list, fmt);
	vsnprintf(str, sizeof(str), fmt, list);
	va_end(list);

	return glGetUniformLocation(program, str);
}

void hz_light_move(struct hz_light *l, float x, float y, float z)
{
	hz_vec3 position = { x, y, z };
	vec3_dup(l->position, position);
}

void hz_light_dim(struct hz_light *l, float intensity)
{
	l->intensity = intensity;
}

bool hz_light_assign(struct hz_light *l, unsigned int index)
{
	struct hz_object *super = hz_cast_object(l);

	l->uniforms.position = glGetUniformLocationf(super->program,
			"lights[%d].position", index);
	if (l->uniforms.position < 0) {
		hz_log_e(tag, "unable to find position uniform");
		return false;
	}

	l->uniforms.intensity = glGetUniformLocationf(super->program,
			"lights[%d].intensity", index);
	if (l->uniforms.intensity < 0) {
		hz_log_e(tag, "unable to find intensity uniform");
		return false;
	}

	l->index = index;
	return true;
}

static void light_draw(struct hz_object *super, struct hz_camera *c)
{
	struct hz_light *l = hz_cast_light(super);

	if (l->index < 0) {
		return;
	}

	glUniform3fv(l->uniforms.position, 1, l->position);
	glUniform1f(l->uniforms.intensity, l->intensity);
}

static bool light_init(struct hz_object *super)
{
	struct hz_light *l = hz_cast_light(super);

	l->index = -1;
	hz_light_move(l, 0.f, 0.f, 0.f);
	hz_light_dim(l, 1.0);

	return true;
}

static void light_deinit(struct hz_object *super)
{
	/* pass */
}

const struct hz_object_proto hz_light_proto = {
	.draw = light_draw,
	.init = light_init,
	.deinit = light_deinit,
};
