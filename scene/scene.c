#include <stdlib.h>
#include <utils/list.h>
#include <utils/common.h>
#include <objects/object.h>

#include "scene.h"

void hz_scene_init(struct hz_scene *s)
{
	list_init(&s->drawing_list);
}

void hz_scene_attach(struct hz_scene *s, struct hz_object *o)
{
	list_push(&o->scene_node, &s->drawing_list);
}

void hz_scene_update_mvp(struct hz_scene *s, mat4x4 vp)
{
	struct list_item *ptr;
	struct hz_object *o;

	list_backward(ptr, &s->drawing_list) {
		o = container_of(ptr, struct hz_object, scene_node);
		hz_object_update_mvp(o, vp);
	}
}

void hz_scene_redraw(struct hz_scene *s, float time)
{
	struct list_item *ptr;
	struct hz_object *o;

	list_backward(ptr, &s->drawing_list) {
		o = container_of(ptr, struct hz_object, scene_node);
		hz_object_draw(o);
	}
}
