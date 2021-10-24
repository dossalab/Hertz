#ifndef GL_MESH_H
#define GL_MESH_H

#include <GL/gl.h>
#include <utils/linmath.h>
#include <utils/defines.h>
#include <stddef.h>
#include <stdbool.h>

struct mesh {
	unsigned int vao, vbo, nbo, tbo;

	/* variables from shaders */
	GLint mvp_handle, model_handle, time_handle;
	bool model_presented, time_presented;

	mat4x4 model;
	mat4x4 mvp;
	int program;
	unsigned int texture;
	bool texture_attached;
	size_t vertex_count;
};

int mesh_attach_textures(struct mesh *m, GLuint texture,
		struct point *uvs, size_t uv_count);
int mesh_create_from_geometry(struct mesh *mesh, GLuint shader_program,
		struct vertex *vertices, size_t vertex_count,
		struct vertex *normals, size_t normal_count);
void mesh_update_mvp(struct mesh *m, mat4x4 pv);
void mesh_redraw(struct mesh *m);
void mesh_free(struct mesh *m);

#endif

