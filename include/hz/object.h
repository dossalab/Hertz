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
	hz_mat4x4 model, local_model;
};

#define HZ_OBJECT(ptr) &(ptr)->super

void hz_object_insert(struct hz_object *o, struct hz_object *child);
void hz_object_move(struct hz_object *o, hz_vec3 pos);
void hz_object_draw(struct hz_object *o, struct hz_camera *c);
void hz_object_update(struct hz_object *o);
void hz_object_init(struct hz_object *o, const struct hz_object_proto *proto);

#endif
