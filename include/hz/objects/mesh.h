#ifndef HZ_OBJECTS_MESH_H
#define HZ_OBJECTS_MESH_H

#include HZ_GL_HEADER
#include <stdbool.h>
#include <stddef.h>
#include <hz/utils/container_of.h>
#include <hz/object.h>
#include <hz/material.h>
#include <hz/types.h>
#include <internal/object.h>

struct hz_mesh {
	struct hz_object super;
	GLuint vao;
	GLuint program;

	struct {
		GLuint vertices, normals, uvs, indices;
	} buffers;

	struct {
		GLint mvp, model;
	} uniforms;

	struct hz_material *material;
	size_t nindices;
};

#define HZ_MESH(ptr) \
	hz_container_of(ptr, struct hz_mesh, super)

bool hz_mesh_set_geometry(struct hz_mesh *o,
		hz_vec3 *vertices, hz_vec3 *normals, size_t nvertices,
		hz_vec3 *uvs, size_t nuvs,
		unsigned *indices, size_t nindices);

bool hz_mesh_init(struct hz_mesh *o, GLuint program, struct hz_material *m);
void hz_mesh_deinit(struct hz_mesh *o);

#endif
