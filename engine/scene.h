#ifndef ENGINE_SCENE_H
#define ENGINE_SCENE_H

#include <utils/list.h>
#include <gl/mesh.h>

struct scene_node {
	struct list_item head;
	struct mesh *mesh;
};

struct scene {
	struct list_item drawing_list;
};

int scene_init(struct scene *s);
int scene_add_mesh(struct scene *s, struct mesh *m);
void scene_update_mvp(struct scene *s, mat4x4 vp);
void scene_redraw(struct scene *s);
void scene_free(struct scene *s);

#endif

