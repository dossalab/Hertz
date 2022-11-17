#include HZ_GL_EXTENSIONS_HEADER
#include <hz/nodes/light.h>
#include <vendor/linmath/linmath.h>
#include <stdio.h>
#include <hz/internal/node.h>
#include <hz/internal/helpers/binders.h>

#define HZ_LIGHT_UNIFORM_PARAMETER_LEN 64

struct _hz_light {
	hz_node super;
	GLuint program;

	struct {
		GLint position, intensity, constant, linear, quadratic;
	} uniforms;

	struct {
		float constant, linear, quadratic;
	} parameters;

	int index;
	float intensity;
};

void hz_light_dim(hz_light *l, float intensity)
{
	l->intensity = intensity;
}

void hz_light_setup(hz_light *l, float kc, float kl, float kq)
{
	l->parameters.constant = kc;
	l->parameters.linear = kl;
	l->parameters.quadratic = kq;
}

static void light_bind(hz_node *super)
{
	hz_light *l = HZ_LIGHT(super);
	vec3 position, scale;
	quat rotation;

	if (l->index < 0) {
		return;
	}

	/* we obviuosly only care about position */
	mat4x4_decompose(super->model, scale, rotation, position);

	glUseProgram(l->program);

	glUniform3fv(l->uniforms.position, 1, position);
	glUniform1f(l->uniforms.intensity, l->intensity);
	glUniform1f(l->uniforms.constant, l->parameters.constant);
	glUniform1f(l->uniforms.linear, l->parameters.linear);
	glUniform1f(l->uniforms.quadratic, l->parameters.quadratic);
}

hz_light *HZ_LIGHT(hz_node *n)
{
	return hz_container_of(n, hz_light, super);
}

static void light_init(hz_node *super, GLuint program, unsigned index)
{
	hz_light *light = HZ_LIGHT(super);
	bool ok;
	typedef char uniform_parameter[HZ_LIGHT_UNIFORM_PARAMETER_LEN];
	uniform_parameter intensity, position, quadratic, constant, linear;

	snprintf(position, sizeof(position), "lights[%d].position", index);
	snprintf(intensity, sizeof(intensity), "lights[%d].intensity", index);
	snprintf(constant, sizeof(constant), "lights[%d].constant", index);
	snprintf(linear, sizeof(linear), "lights[%d].linear", index);
	snprintf(quadratic, sizeof(quadratic), "lights[%d].quadratic", index);

	struct hz_uniform_binding bindings[] = {
		{ &light->uniforms.position, position },
		{ &light->uniforms.intensity, intensity },
		{ &light->uniforms.constant, constant },
		{ &light->uniforms.linear, linear },
		{ &light->uniforms.quadratic, quadratic },
	};

	ok = hz_bind_uniforms(bindings, program, HZ_ARRAY_SIZE(bindings));
	if (!ok) {
		return;
	}

	light->index = index;
	light->program = program;

	hz_light_dim(light, 1.0);
	hz_light_setup(light, 1.0f, 0.09f, 0.032f);
}

const hz_node_proto hz_light_proto = {
	.bind = light_bind,
	.draw = hz_node_dummy_draw,
	.arena_proto = {
		.name = "lights",
		.size = sizeof(hz_light)
	}
};

hz_node *hz_light_new(hz_arena *arena, GLuint program, unsigned index)
{
	hz_node *super = hz_node_new(arena, hz_light, &hz_light_proto);
	if (!super) {
		return NULL;
	}

	light_init(super, program, index);

	return super;
}
