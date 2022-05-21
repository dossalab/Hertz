#ifndef HZ_INTERNAL_OBJECT_H
#define HZ_INTERNAL_OBJECT_H

#include <hz/types.h>
#include <hz/adt/tree.h>

struct hz_object_proto {
	void (*bind)(struct hz_object *o);
	void (*draw)(struct hz_object *o, struct hz_camera *c);
};

struct hz_object {
	struct hz_tree_node scene_node;
	const struct hz_object_proto *proto;
	hz_mat4x4 model, local_model;
};

void hz_object_init(struct hz_object *o, const struct hz_object_proto *proto);

#endif
