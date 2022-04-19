#ifndef OBJECTS_BASIC_H
#define OBJECTS_BASIC_H

#include <GL/gl.h>
#include <utils/3rdparty/linmath/linmath.h>
#include <stdbool.h>
#include <stddef.h>
#include <hz/utils/container_of.h>
#include <hz/object.h>

struct hz_basic_object {
	struct hz_object as_object;

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

#define hz_cast_basic_object(ptr) \
	hz_container_of(ptr, struct hz_basic_object, as_object)

bool hz_basic_object_set_texture(struct hz_basic_object *o, GLuint texture,
		vec3 *uvs, size_t uv_count);

bool hz_basic_object_set_geometry(struct hz_basic_object *o,
		vec3 *vertices, vec3 *normals, size_t nvertices,
		unsigned *indices, size_t nindices);

extern const struct hz_object_proto hz_basic_object_proto;

#endif
