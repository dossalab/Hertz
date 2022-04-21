#include GL_EXTENSIONS_HEADER
#include <hz/objects/light.h>
#include <hz/logger.h>
#include <vendor/linmath/linmath.h>

static const char *tag = "light";

void hz_light_move(struct hz_light *l, float x, float y, float z)
{
	hz_vec3 position = { x, y, z };
	vec3_dup(l->position, position);
}

static void light_draw(struct hz_object *super, struct hz_camera *c)
{
	struct hz_light *l = hz_cast_light(super);
	glUniform3fv(l->uniforms.position, 1, l->position);
}

static bool light_init(struct hz_object *super)
{
	struct hz_light *l = hz_cast_light(super);

	hz_light_move(l, 0.f, 0.f, 0.f);

	l->uniforms.position = glGetUniformLocation(super->program, "light.position");
	if (l->uniforms.position < 0) {
		hz_log_e(tag, "unable to find uniform position");
		return false;
	}

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
