#ifndef SCENE_SCENE_H
#define SCENE_SCENE_H

#include <utils/list.h>
#include <stdbool.h>
#include <utils/3rdparty/linmath/linmath.h>

struct object;

struct scene {
	struct list_item drawing_list;
};

void scene_init(struct scene *s);
void scene_attach(struct scene *s, struct object *o);
void scene_update_mvp(struct scene *s, mat4x4 vp);
void scene_redraw(struct scene *s, float time);

#endif

