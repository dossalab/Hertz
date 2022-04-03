#ifndef OBJECTS_MESH_H
#define OBJECTS_MESH_H

#include <GL/gl.h>
#include <utils/linmath.h>
#include <stdbool.h>

struct mesh {
	GLuint vao, vbo, nbo, tbo;

	/* variables from shaders */
	GLint mvp_handle, model_handle, time_handle;
	bool model_presented, time_presented, nbo_presented;

	mat4x4 model;
	mat4x4 mvp;
	GLuint program;
	GLuint texture;

	bool texture_attached;
	size_t vertex_count;
};

void mesh_update_mvp(struct mesh *m, mat4x4 pv);
void mesh_redraw(struct mesh *m, float time);

bool mesh_texture(struct mesh *m, GLuint texture, vec2 *uvs, size_t uv_count);

bool mesh_create_from_geometry(struct mesh *mesh, GLuint shader_program,
		vec3 *vertices, size_t vertex_count,
		vec3 *normals, size_t normal_count);

void mesh_free(struct mesh *m);

#endif
