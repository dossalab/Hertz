#ifndef ENGINE_MESH_H
#define ENGINE_MESH_H

#include <utils/linmath.h>
#include <GL/gl.h>

struct point {
	float x, y;
};

struct vertex {
	float x, y, z;
};

struct mesh {
	unsigned int vao, vbo, nbo, tbo;
	struct vertex *vertices;
	int vertice_cnt;

	struct vertex *normals;
	int normal_cnt;

	struct point *tex_coords;
	int tex_coord_cnt;

	mat4x4 model;
	int program;
	unsigned int texture;

	char *texture_path;
};

int mesh_load(struct mesh *m, char *path, GLuint shader_program);
void mesh_update_mvp(struct mesh *m, mat4x4 pv);
void mesh_redraw(struct mesh *m);
void mesh_free(struct mesh *m);

#endif

