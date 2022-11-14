#pragma once

#include <hz/types.h>
#include <hz/adt/tree.h>
#include <hz/nodes/camera.h>

 /*
 * Most likely users of this header will inherit from hz_node, so
 * include this for convenient access to hz_container_of
 */
#include <hz/utils.h>

typedef struct _hz_node hz_node;
typedef struct {
	void (*bind)(hz_node *n);
	void (*draw)(hz_node *n, hz_camera *c);
} hz_node_proto;

struct _hz_node {
	hz_tree_node scene_node;
	const hz_node_proto *proto;
	hz_mat4x4 model, local_model;
	hz_vec3 position, scale;
	hz_quat rotation;
};

#define HZ_NODE(ptr) &(ptr)->super

void hz_node_init(hz_node *n, const hz_node_proto *proto);