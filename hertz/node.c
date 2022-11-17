#include <hz/node.h>
#include <hz/internal/node.h>
#include <vendor/linmath/linmath.h>
#include <hz/nodes/root.h>
#include <hz/nodes/camera.h>

static void calculate_local_model(hz_node *o)
{
	mat4x4 rotation, position;

	mat4x4_from_quat(rotation, o->rotation);
	mat4x4_translate(position, o->position[0], o->position[1], o->position[2]);

	mat4x4_mul(o->local_model, position, rotation);
	mat4x4_scale_aniso(o->local_model, o->local_model, \
			o->scale[0], o->scale[1], o->scale[2]);
}

static void scene_node_update_callback(hz_tree_node *parent_node,
		hz_tree_node *node, void *user)
{
	hz_node *scene_node, *parent;

	scene_node = hz_container_of(node, hz_node, scene_node);

	if (parent_node) {
		parent = hz_container_of(parent_node, hz_node, scene_node);
		mat4x4_mul(scene_node->model, parent->model, scene_node->local_model);
	} else {
		mat4x4_dup(scene_node->model, scene_node->local_model);
	}
}

static void scene_node_draw_callback(hz_tree_node *parent,
		hz_tree_node *node, void *user)
{
	hz_node *scene_node;
	hz_camera *c = user;

	scene_node = hz_container_of(node, hz_node, scene_node);
	scene_node->proto->draw(scene_node, c);
}

static void scene_node_bind_callback(hz_tree_node *parent,
		hz_tree_node *node, void *user)
{
	hz_node *scene_node;

	scene_node = hz_container_of(node, hz_node, scene_node);
	scene_node->proto->bind(scene_node);
}

void hz_node_insert(hz_node *node, hz_node *child)
{
	hz_tree_insert(&node->scene_node, &child->scene_node);
	hz_node_update(node);
}

void hz_node_move(hz_node *node, vec3 pos)
{
	vec3_dup(node->position, pos);
	calculate_local_model(node);
	hz_node_update(node);
}

void hz_node_rotate_quat(hz_node *node, hz_quat q)
{
	vec4_dup(node->rotation, q);
	calculate_local_model(node);
	hz_node_update(node);
}

void hz_node_rotate(hz_node *node, float angle, vec3 axis)
{
	quat_rotate(node->rotation, angle, axis);
	calculate_local_model(node);
	hz_node_update(node);
}

void hz_node_scale(hz_node *node, vec3 scale)
{
	vec3_dup(node->scale, scale);
	calculate_local_model(node);
	hz_node_update(node);
}

void hz_node_set_model(hz_node *node, hz_mat4x4 model, bool transpose)
{
	if (transpose) {
		mat4x4_transpose(node->local_model, model);
	} else {
		mat4x4_dup(node->local_model, model);
	}

	mat4x4_decompose(node->local_model, node->scale, node->rotation, node->position);
	calculate_local_model(node);

	hz_node_update(node);
}

void hz_node_draw(hz_node *node, hz_camera *c)
{
	hz_tree_traverse(NULL, &node->scene_node, scene_node_bind_callback, c);
	hz_tree_traverse(NULL, &node->scene_node, scene_node_draw_callback, c);
}

void hz_node_update(hz_node *node)
{
	hz_tree_traverse(NULL, &node->scene_node, scene_node_update_callback, NULL);
}

void hz_node_dummy_bind(hz_node *node)
{

}

void hz_node_dummy_draw(hz_node *node, hz_camera *camera)
{

}

void hz_node_init(hz_node *node, const hz_node_proto *proto)
{
	node->proto = proto;

	hz_tree_init(&node->scene_node);
	mat4x4_identity(node->model);

	hz_node_move(node, (vec3) { 0.f, 0.f, 0.f });
	hz_node_rotate(node, 0.f, (vec3) { 1.f, 0.f, 0.f });
	hz_node_scale(node, (vec3) { 1.f, 1.f, 1.f });
}
