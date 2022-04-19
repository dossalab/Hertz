#include <stdlib.h>
#include <utils/list.h>
#include <utils/common.h>
#include <objects/object.h>

#include "scene.h"

void scene_init(struct scene *s)
{
	list_init(&s->drawing_list);
}

void scene_attach(struct scene *s, struct object *o)
{
	list_push(&o->scene_node, &s->drawing_list);
}

void scene_update_mvp(struct scene *s, mat4x4 vp)
{
	struct list_item *ptr;
	struct object *o;

	list_backward(ptr, &s->drawing_list) {
		o = container_of(ptr, struct object, scene_node);
		object_update_mvp(o, vp);
	}
}

void scene_redraw(struct scene *s, float time)
{
	struct list_item *ptr;
	struct object *o;

	list_backward(ptr, &s->drawing_list) {
		o = container_of(ptr, struct object, scene_node);
		object_draw(o);
	}
}
