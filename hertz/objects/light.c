#include GL_EXTENSIONS_HEADER
#include <hz/objects/light.h>
#include <hz/logger.h>
#include <vendor/linmath/linmath.h>
#include <hz/helpers/binders.h>
#include <stdio.h>

void hz_light_move(struct hz_light *l, float x, float y, float z)
{
	hz_vec3 position = { x, y, z };
	vec3_dup(l->position, position);
}

void hz_light_dim(struct hz_light *l, float intensity)
{
	l->intensity = intensity;
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

static bool light_probe(struct hz_object *super)
{
	struct hz_light *l = hz_cast_light(super);
	char intensity[64], position[64];
	bool ok;

	snprintf(position, sizeof(position), "lights[%d].position", l->index);
	snprintf(intensity, sizeof(intensity), "lights[%d].intensity", l->index);

	struct hz_uniform_binding bindings[] = {
		{ &l->uniforms.position, position },
		{ &l->uniforms.intensity, intensity },
	};

	ok = hz_bind_uniforms(bindings, super->program, HZ_ARRAY_SIZE(bindings));
	if (!ok) {
		return false;
	}

	hz_light_move(l, 0.f, 0.f, 0.f);
	hz_light_dim(l, 1.0);

	return true;
}

static void light_remove(struct hz_object *super)
{
	/* pass */
}

const struct hz_object_proto hz_light_proto = {
	.draw = light_draw,
	.probe = light_probe,
	.remove = light_remove,
};

bool hz_light_init(struct hz_light *l, GLuint program, unsigned int index)
{
	struct hz_object *super = hz_cast_object(l);

	l->index = index;
	return hz_object_init(super, program, &hz_light_proto);
}
