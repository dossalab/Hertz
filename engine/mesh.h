#ifndef ENGINE_MESH_H
#define ENGINE_MESH_H

#include <utils/linmath.h>
#include <GL/gl.h>
#include "defines.h"
#include <stddef.h>

struct mesh {
	unsigned int vao, vbo, nbo, tbo;
	mat4x4 model;
	int program;
	unsigned int texture;
	size_t vertex_count;
};

int mesh_load(struct mesh *m, char *path, GLuint shader_program);
void mesh_update_mvp(struct mesh *m, mat4x4 pv);
void mesh_redraw(struct mesh *m);
void mesh_free(struct mesh *m);

#endif

