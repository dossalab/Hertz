#ifndef OBJECTS_BASIC_H
#define OBJECTS_BASIC_H

#include <GL/gl.h>
#include <utils/3rdparty/linmath/linmath.h>
#include <stdbool.h>
#include <stddef.h>
#include <render/object.h>
#include <utils/common.h>

struct basic_object {
	struct object as_object;

	GLuint vbo, nbo, tbo, ebo;

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

#define cast_basic_object(ptr) \
	container_of(ptr, struct basic_object, as_object)

bool basic_object_texture(struct basic_object *o, GLuint texture, vec3 *uvs, size_t uv_count);

bool basic_object_create_from_geometry(struct basic_object *o, GLuint shader_program,
		vec3 *vertices, vec3 *normals, size_t nvertices,
		unsigned *indices, size_t nindices);

#endif
