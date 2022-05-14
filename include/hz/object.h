#ifndef HZ_OBJECT_H
#define HZ_OBJECT_H

#include <GL/gl.h>
#include <stdbool.h>
#include <hz/types.h>
#include <hz/adt/tree.h>
#include "camera.h"

struct hz_object;

struct hz_object_proto {
	void (*draw)(struct hz_object *o, struct hz_camera *c);
};

struct hz_object {
	struct hz_tree_node scene_node;
	const struct hz_object_proto *proto;
};

#define hz_cast_object(ptr) &(ptr)->super

void hz_object_insert(struct hz_object *o, struct hz_object *child);

void hz_object_draw(struct hz_object *o, struct hz_camera *c);
void hz_object_init(struct hz_object *o, const struct hz_object_proto *proto);

#endif
