#include <stdlib.h>

#include <hz/adt/list.h>
#include <hz/utils/container_of.h>
#include <vendor/linmath/linmath.h>
#include <hz/object.h>
#include <hz/scene.h>
#include <hz/camera.h>

void hz_scene_init(struct hz_scene *s)
{
	hz_list_init(&s->drawing_list);
}

void hz_scene_attach(struct hz_scene *s, struct hz_object *o)
{
	hz_list_push(&o->scene_node, &s->drawing_list);
}

void hz_scene_redraw(struct hz_scene *s, struct hz_camera *c, float time)
{
	struct hz_list_item *ptr;
	struct hz_object *o;

	hz_list_backward(ptr, &s->drawing_list) {
		o = hz_container_of(ptr, struct hz_object, scene_node);
		hz_object_draw(o, c);
	}
}
