#ifndef RENDER_OBJECT_H
#define RENDER_OBJECT_H

#include <GL/gl.h>
#include <utils/3rdparty/linmath/linmath.h>
#include <stdbool.h>
#include <stddef.h>

struct basic_object {
	GLuint vao, vbo, nbo, tbo, ebo;

	/* variables from shaders */
	GLint mvp_handle, model_handle, time_handle;
	bool model_presented, time_presented, nbo_presented;

	mat4x4 model;
	mat4x4 mvp;
	GLuint program;
	GLuint texture;

	bool texture_attached;
	unsigned nindices;
};

void basic_object_update_mvp(struct basic_object *o, mat4x4 pv);
void basic_object_redraw(struct basic_object *o, float time);

bool basic_object_texture(struct basic_object *o, GLuint texture, vec3 *uvs, size_t uv_count);

bool basic_object_create_from_geometry(struct basic_object *o, GLuint shader_program,
		vec3 *vertices, vec3 *normals, size_t nvertices,
		unsigned *indices, size_t nindices);

void basic_object_free(struct basic_object *o);

#endif
