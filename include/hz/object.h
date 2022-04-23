#ifndef HZ_OBJECT_H
#define HZ_OBJECT_H

#include <GL/gl.h>
#include <stdbool.h>
#include <hz/types.h>
#include "scene.h"

struct hz_object;

struct hz_object_proto {
	void (*draw)(struct hz_object *o, struct hz_camera *c);
	bool (*probe)(struct hz_object *o);
	void (*remove)(struct hz_object *o);
};

struct hz_object {
	GLuint vao;
	GLuint program;
	struct hz_list_item scene_node;
	const struct hz_object_proto *proto;
};

#define hz_cast_object(ptr) &(ptr)->super

void hz_object_draw(struct hz_object *o, struct hz_camera *c);
bool hz_object_init(struct hz_object *o, GLuint program,
		const struct hz_object_proto *proto);
void hz_object_deinit(struct hz_object *o);

#endif
