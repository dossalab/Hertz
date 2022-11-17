#pragma once

#include <hz/types.h>
#include <hz/adt/tree.h>
#include <hz/nodes/camera.h>

 /*
 * Most likely users of this header will inherit from hz_node, so
 * include this for convenient access to hz_container_of
 */
#include <hz/util/common.h>
#include <hz/util/arena.h>

typedef struct _hz_node hz_node;
typedef struct {
	void (*bind)(hz_node *n);
	void (*draw)(hz_node *n, hz_camera *c);
	const hz_arena_proto arena_proto;
} hz_node_proto;

struct _hz_node {
	hz_tree_node scene_node;
	const hz_node_proto *proto;
	hz_mat4x4 model, local_model;
	hz_vec3 position, scale;
	hz_quat rotation;
};

#define HZ_NODE(ptr) &(ptr)->super

void hz_node_dummy_bind(hz_node *node);
void hz_node_dummy_draw(hz_node *node, hz_camera *camera);
void hz_node_init(hz_node *node, const hz_node_proto *proto);

#define hz_node_new(arena, type, proto) ({ \
	type *node = hz_arena_alloc(arena, &(proto)->arena_proto); \
	if (!node) { \
		return NULL; \
	} \
	hz_node_init(HZ_NODE(node), proto); \
	HZ_NODE(node); \
})
