#ifndef HZ_OBJECTS_BASIC_H
#define HZ_OBJECTS_BASIC_H

#include <GL/gl.h>
#include <stdbool.h>
#include <hz/utils/container_of.h>
#include <hz/object.h>
#include <hz/types.h>

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

#define hz_cast_light(ptr) \
	hz_container_of(ptr, struct hz_light, super)

void hz_light_setup(struct hz_light *l, float kc, float kl, float kq);
void hz_light_dim(struct hz_light *l, float intensity);
bool hz_light_init(struct hz_light *l, GLuint program, unsigned index);

#endif
