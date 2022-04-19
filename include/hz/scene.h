#ifndef HZ_SCENE_H
#define HZ_SCENE_H

#include <vendor/linmath/linmath.h>
#include <hz/adt/list.h>

struct hz_object;

struct hz_scene {
	struct hz_list_item drawing_list;
};

void hz_scene_init(struct hz_scene *s);
void hz_scene_attach(struct hz_scene *s, struct hz_object *o);
void hz_scene_update_mvp(struct hz_scene *s, mat4x4 vp);
void hz_scene_redraw(struct hz_scene *s, float time);

#endif

