#ifndef OBJECTS_BASIC_H
#define OBJECTS_BASIC_H

#include <GL/gl.h>
#include <utils/3rdparty/linmath/linmath.h>
#include <stdbool.h>
#include <stddef.h>
#include <utils/common.h>
#include "object.h"

struct basic_object {
	struct object as_object;

	struct {
		GLuint vertices, normals, uvs, indices;
	} buffers;

	struct {
		GLint mvp, model;
	} uniforms;

	struct {
		mat4x4 model;
		mat4x4 mvp;
	} transform;

	GLuint texture;

	bool texture_attached;
	size_t nindices;
};

#define cast_basic_object(ptr) \
	container_of(ptr, struct basic_object, as_object)

bool basic_object_set_texture(struct basic_object *o, GLuint texture,
		vec3 *uvs, size_t uv_count);

bool basic_object_set_geometry(struct basic_object *o,
		vec3 *vertices, vec3 *normals, size_t nvertices,
		unsigned *indices, size_t nindices);

extern const struct object_proto basic_object_proto;

#endif
