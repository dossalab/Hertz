#ifndef HZ_OBJECTS_BASIC_H
#define HZ_OBJECTS_BASIC_H

#include <GL/gl.h>
#include <stdbool.h>
#include <hz/utils/container_of.h>
#include <hz/object.h>
#include <hz/types.h>

struct hz_light {
	struct hz_object super;

	struct {
		GLint position, intensity;
	} uniforms;

	int index;
	float intensity;
	hz_vec3 position;
};

#define hz_cast_light(ptr) \
	hz_container_of(ptr, struct hz_light, super)

void hz_light_dim(struct hz_light *l, float intensity);
void hz_light_move(struct hz_light *l, hz_vec3 position);
bool hz_light_init(struct hz_light *l, GLuint program, unsigned index);

#endif
