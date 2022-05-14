#include <hz/object.h>
#include <hz/camera.h>
#include <hz/utils/container_of.h>

void hz_object_insert(struct hz_object *o, struct hz_object *child)
{
	hz_tree_insert(&o->scene_node, &child->scene_node);
}

static void scene_node_traverse_callback(struct hz_tree_node *node, void *user)
{
	struct hz_object *o;
	struct hz_camera *c = user;

	o = hz_container_of(node, struct hz_object, scene_node);
	o->proto->draw(o, c);
}

void hz_object_draw(struct hz_object *o, struct hz_camera *c)
{
	hz_tree_traverse(&o->scene_node, scene_node_traverse_callback, c);
}

void hz_object_init(struct hz_object *o, const struct hz_object_proto *proto)
{
	o->proto = proto;
	hz_tree_init(&o->scene_node);
}
