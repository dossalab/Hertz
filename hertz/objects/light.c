#include GL_EXTENSIONS_HEADER
#include <hz/objects/light.h>
#include <hz/logger.h>
#include <vendor/linmath/linmath.h>
#include <hz/helpers/binders.h>
#include <stdio.h>

#define HZ_LIGHT_UNIFORM_PARAMETER_LEN 64

void hz_light_move(struct hz_light *l, vec3 position)
{
	vec3_dup(l->position, position);
}

void hz_light_dim(struct hz_light *l, float intensity)
{
	l->intensity = intensity;
}

void hz_light_setup(struct hz_light *l, float kc, float kl, float kq)
{
	l->parameters.constant = kc;
	l->parameters.linear = kl;
	l->parameters.quadratic = kq;
}

static void light_draw(struct hz_object *super, struct hz_camera *c)
{
	struct hz_light *l = hz_cast_light(super);

	if (l->index < 0) {
		return;
	}

	glUniform3fv(l->uniforms.position, 1, l->position);
	glUniform1f(l->uniforms.intensity, l->intensity);
	glUniform1f(l->uniforms.constant, l->parameters.constant);
	glUniform1f(l->uniforms.linear, l->parameters.linear);
	glUniform1f(l->uniforms.quadratic, l->parameters.quadratic);
}

static bool light_probe(struct hz_object *super)
{
	struct hz_light *l = hz_cast_light(super);
	bool ok;

	typedef char uniform_parameter[HZ_LIGHT_UNIFORM_PARAMETER_LEN];
	uniform_parameter intensity, position, quadratic, constant, linear;

	snprintf(position, sizeof(position), "lights[%d].position", l->index);
	snprintf(intensity, sizeof(intensity), "lights[%d].intensity", l->index);
	snprintf(constant, sizeof(constant), "lights[%d].constant", l->index);
	snprintf(linear, sizeof(linear), "lights[%d].linear", l->index);
	snprintf(quadratic, sizeof(quadratic), "lights[%d].quadratic", l->index);

	struct hz_uniform_binding bindings[] = {
		{ &l->uniforms.position, position },
		{ &l->uniforms.intensity, intensity },
		{ &l->uniforms.constant, constant },
		{ &l->uniforms.linear, linear },
		{ &l->uniforms.quadratic, quadratic },
	};

	ok = hz_bind_uniforms(bindings, super->program, HZ_ARRAY_SIZE(bindings));
	if (!ok) {
		return false;
	}

	hz_light_move(l, (vec3) { 0.f, 0.f, 0.f });
	hz_light_dim(l, 1.0);
	hz_light_setup(l, 1.0f, 0.09f, 0.032f);

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

bool hz_light_init(struct hz_light *l, GLuint program, unsigned index)
{
	struct hz_object *super = hz_cast_object(l);

	l->index = index;
	return hz_object_init(super, program, &hz_light_proto);
}
