#include HZ_GL_EXTENSIONS_HEADER
#include <hz/objects/light.h>
#include <vendor/linmath/linmath.h>
#include <stdio.h>
#include <internal/object.h>
#include <internal/alloc.h>
#include <internal/helpers/binders.h>
#include <hz/utils/container_of.h>

#define HZ_LIGHT_UNIFORM_PARAMETER_LEN 64

struct hz_light {
	struct hz_object super;
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

static void light_bind(struct hz_object *super)
{
	struct hz_light *l = HZ_LIGHT(super);
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

static void light_draw(struct hz_object *super, struct hz_camera *c)
{
	/* pass */
}

const struct hz_object_proto hz_light_proto = {
	.bind = light_bind,
	.draw = light_draw,
};

static bool light_init(struct hz_light *l, GLuint program, unsigned index)
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

struct hz_light *HZ_LIGHT(struct hz_object *o)
{
	return hz_container_of(o, struct hz_light, super);
}

struct hz_object *hz_light_new(GLuint program, unsigned index)
{
	return HZ_OBJECT(hz_alloc_and_init(struct hz_light, light_init, program, index));
}
