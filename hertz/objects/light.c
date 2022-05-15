#include GL_EXTENSIONS_HEADER
#include <hz/objects/light.h>
#include <hz/logger.h>
#include <vendor/linmath/linmath.h>
#include <hz/helpers/binders.h>
#include <stdio.h>

#define HZ_LIGHT_UNIFORM_PARAMETER_LEN 64

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
	struct hz_light *l = HZ_LIGHT(super);
	vec4 position = { 0.f, 0.f, 0.f, 1.f };

	if (l->index < 0) {
		return;
	}

	mat4x4_mul_vec4(position, super->model, position);

	glUseProgram(l->program);

	/* load it as vec3, ignoring the w component */
	glUniform3fv(l->uniforms.position, 1, position);

	glUniform1f(l->uniforms.intensity, l->intensity);
	glUniform1f(l->uniforms.constant, l->parameters.constant);
	glUniform1f(l->uniforms.linear, l->parameters.linear);
	glUniform1f(l->uniforms.quadratic, l->parameters.quadratic);
}

const struct hz_object_proto hz_light_proto = {
	.draw = light_draw,
};

bool hz_light_init(struct hz_light *l, GLuint program, unsigned index)
{
	struct hz_object *super = HZ_OBJECT(l);
	bool ok;
	typedef char uniform_parameter[HZ_LIGHT_UNIFORM_PARAMETER_LEN];
	uniform_parameter intensity, position, quadratic, constant, linear;

	hz_object_init(super, &hz_light_proto);

	snprintf(position, sizeof(position), "lights[%d].position", index);
	snprintf(intensity, sizeof(intensity), "lights[%d].intensity", index);
	snprintf(constant, sizeof(constant), "lights[%d].constant", index);
	snprintf(linear, sizeof(linear), "lights[%d].linear", index);
	snprintf(quadratic, sizeof(quadratic), "lights[%d].quadratic", index);

	struct hz_uniform_binding bindings[] = {
		{ &l->uniforms.position, position },
		{ &l->uniforms.intensity, intensity },
		{ &l->uniforms.constant, constant },
		{ &l->uniforms.linear, linear },
		{ &l->uniforms.quadratic, quadratic },
	};

	ok = hz_bind_uniforms(bindings, program, HZ_ARRAY_SIZE(bindings));
	if (!ok) {
		return false;
	}

	l->index = index;
	l->program = program;

	hz_light_dim(l, 1.0);
	hz_light_setup(l, 1.0f, 0.09f, 0.032f);

	return true;
}

