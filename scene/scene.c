#include <stdlib.h>
#include <utils/list.h>
#include <utils/common.h>

#include "scene.h"

void scene_init(struct scene *s)
{
	list_init(&s->drawing_list);
}

bool scene_add_mesh(struct scene *s, struct mesh *m)
{
	struct scene_node *node = malloc(sizeof(struct scene_node));
	if (!node) {
		return false;
	}

	node->mesh = m;

	list_push(&node->head, &s->drawing_list);
	return true;
}

void scene_update_mvp(struct scene *s, mat4x4 vp)
{
	struct list_item *ptr;
	struct scene_node *node;
	struct mesh *mesh;

	list_backward(ptr, &s->drawing_list) {
		node = container_of(ptr, struct scene_node, head);
		mesh = node->mesh;

		mesh_update_mvp(mesh, vp);
	}
}

void scene_redraw(struct scene *s, float time)
{
	struct list_item *ptr;
	struct scene_node *node;
	struct mesh *mesh;

	list_backward(ptr, &s->drawing_list) {
		node = container_of(ptr, struct scene_node, head);
		mesh = node->mesh;

		mesh_redraw(mesh, time);
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

