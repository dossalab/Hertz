#ifndef RENDER_SCENE_H
#define RENDER_SCENE_H

#include <utils/list.h>
#include <stdbool.h>
#include <utils/3rdparty/linmath/linmath.h>

struct basic_object;

struct scene_node {
	struct list_item head;
	struct basic_object *object;
};

struct scene {
	struct list_item drawing_list;
};

void scene_init(struct scene *s);
bool scene_add_object(struct scene *s, struct basic_object *o);
void scene_update_mvp(struct scene *s, mat4x4 vp);
void scene_redraw(struct scene *s, float time);
void scene_free(struct scene *s);

#endif

