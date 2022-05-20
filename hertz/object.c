#include <hz/object.h>
#include <hz/camera.h>
#include <hz/utils/container_of.h>
#include <vendor/linmath/linmath.h>
#include <hz/objects/root.h>
#include <hz/utils/compiler.h>

static void scene_node_update_callback(struct hz_tree_node *parent_node,
		struct hz_tree_node *node, void *user)
{
	struct hz_object *o, *parent;

	o = hz_container_of(node, struct hz_object, scene_node);

	if (hz_likely(parent_node)) {
		parent = hz_container_of(parent_node, struct hz_object, scene_node);
		mat4x4_mul(o->model, parent->model, o->local_model);
	} else {
		mat4x4_dup(o->model, o->local_model);
	}
}

static void scene_node_draw_callback(struct hz_tree_node *parent,
		struct hz_tree_node *node, void *user)
{
	struct hz_object *o;
	struct hz_camera *c = user;

	o = hz_container_of(node, struct hz_object, scene_node);
	o->proto->draw(o, c);
}

static void scene_node_bind_callback(struct hz_tree_node *parent,
		struct hz_tree_node *node, void *user)
{
	struct hz_object *o;

	o = hz_container_of(node, struct hz_object, scene_node);
	o->proto->bind(o);
}

void hz_object_insert(struct hz_object *o, struct hz_object *child)
{
	hz_tree_insert(&o->scene_node, &child->scene_node);
	hz_object_update(o);
}

void hz_object_move(struct hz_object *o, vec3 pos)
{
	mat4x4_translate(o->local_model, pos[0], pos[1], pos[2]);
	hz_object_update(o);
}

void hz_object_set_model(struct hz_object *o, hz_mat4x4 model, bool transpose)
{
	if (transpose) {
		mat4x4_transpose(o->local_model, model);
	} else {
		mat4x4_dup(o->local_model, model);
	}
}

void hz_object_draw(struct hz_object *o, struct hz_camera *c)
{
	hz_tree_traverse(NULL, &o->scene_node, scene_node_bind_callback, c);
	hz_tree_traverse(NULL, &o->scene_node, scene_node_draw_callback, c);
}

void hz_object_update(struct hz_object *o)
{
	hz_tree_traverse(NULL, &o->scene_node, scene_node_update_callback, NULL);
}

void hz_object_init(struct hz_object *o, const struct hz_object_proto *proto)
{
	o->proto = proto;
	hz_tree_init(&o->scene_node);

	mat4x4_identity(o->model);
	mat4x4_identity(o->local_model);
}
