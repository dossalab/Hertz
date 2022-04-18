#include <stdlib.h>
#include <utils/list.h>
#include <utils/common.h>

#include <objects/basic.h>
#include "scene.h"

void scene_init(struct scene *s)
{
	list_init(&s->drawing_list);
}

bool scene_add_object(struct scene *s, struct basic_object *o)
{
	struct scene_node *node = malloc(sizeof(struct scene_node));
	if (!node) {
		return false;
	}

	node->object = o;

	list_push(&node->head, &s->drawing_list);
	return true;
}

void scene_update_mvp(struct scene *s, mat4x4 vp)
{
	struct list_item *ptr;
	struct scene_node *node;
	struct basic_object *o;

	list_backward(ptr, &s->drawing_list) {
		node = container_of(ptr, struct scene_node, head);
		o = node->object;

		basic_object_update_mvp(o, vp);
	}
}

void scene_redraw(struct scene *s, float time)
{
	struct list_item *ptr;
	struct scene_node *node;
	struct basic_object *o;

	list_backward(ptr, &s->drawing_list) {
		node = container_of(ptr, struct scene_node, head);
		o = node->object;

		basic_object_redraw(o, time);
	}
}

void scene_free(struct scene *s)
{
	struct list_item *ptr, *n;
	struct scene_node *node;

	list_backward_safe(ptr, n, &s->drawing_list) {
		node = container_of(ptr, struct scene_node, head);
		free(node);
	}
}

