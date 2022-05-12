#ifndef HZ_OBJECT_H
#define HZ_OBJECT_H

#include <GL/gl.h>
#include <stdbool.h>
#include <hz/types.h>
#include "scene.h"

struct hz_object;

struct hz_object_proto {
	void (*draw)(struct hz_object *o, struct hz_camera *c);
};

struct hz_object {
	struct hz_list_item scene_node;
	const struct hz_object_proto *proto;
};

#define hz_cast_object(ptr) &(ptr)->super

void hz_object_set_proto(struct hz_object *o, const struct hz_object_proto *proto);
void hz_object_proto_draw(struct hz_object *o, struct hz_camera *c);

#endif
