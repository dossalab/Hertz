#ifndef HZ_OBJECTS_BASIC_H
#define HZ_OBJECTS_BASIC_H

#include <GL/gl.h>
#include <stdbool.h>
#include <stddef.h>
#include <hz/utils/container_of.h>
#include <hz/object.h>
#include <hz/material.h>
#include <hz/types.h>

struct hz_basic_object {
	struct hz_object super;
	GLuint vao;
	GLuint program;

	struct {
		GLuint vertices, normals, uvs, indices;
	} buffers;

	struct {
		GLint mvp, model;
	} uniforms;

	struct {
		hz_mat4x4 model, mvp;
	} transform;

	struct hz_material *material;
	size_t nindices;
};

#define hz_cast_basic_object(ptr) \
	hz_container_of(ptr, struct hz_basic_object, super)

bool hz_basic_object_set_geometry(struct hz_basic_object *o,
		hz_vec3 *vertices, hz_vec3 *normals, size_t nvertices,
		hz_vec3 *uvs, size_t nuvs,
		unsigned *indices, size_t nindices);

bool hz_basic_object_init(struct hz_basic_object *o, GLuint program,
		struct hz_material *m);

#endif
