#ifndef HZ_SCENE_H
#define HZ_SCENE_H

#include <hz/types.h>
#include <hz/adt/list.h>

struct hz_object;
struct hz_camera;

struct hz_scene {
	struct hz_list_item drawing_list;
};

void hz_scene_init(struct hz_scene *s);
void hz_scene_attach(struct hz_scene *s, struct hz_object *o);
void hz_scene_draw(struct hz_scene *s, struct hz_camera *c, float time);

#endif

