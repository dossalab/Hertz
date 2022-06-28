#ifndef HZ_OBJECTS_MESH_H
#define HZ_OBJECTS_MESH_H

#include HZ_GL_HEADER
#include <stdbool.h>
#include <stddef.h>
#include <hz/types.h>

struct hz_mesh;
struct hz_object;
struct hz_material;

bool hz_mesh_set_geometry(struct hz_mesh *o,
		hz_vec3 *vertices, hz_vec3 *normals, size_t nvertices,
		hz_vec3 *uvs, size_t nuvs,
		unsigned *indices, size_t nindices);

struct hz_mesh *HZ_MESH(struct hz_object *o);
struct hz_object *hz_mesh_new(GLuint program, struct hz_material *m);

#endif
