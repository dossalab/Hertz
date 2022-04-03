#ifndef SCENE_SCENE_H
#define SCENE_SCENE_H

#include <utils/list.h>
#include <objects/mesh.h>

struct scene_node {
	struct list_item head;
	struct mesh *mesh;
};

struct scene {
	struct list_item drawing_list;
};

void scene_init(struct scene *s);
bool scene_add_mesh(struct scene *s, struct mesh *m);
void scene_update_mvp(struct scene *s, mat4x4 vp);
void scene_redraw(struct scene *s, float time);
void scene_free(struct scene *s);

#endif

