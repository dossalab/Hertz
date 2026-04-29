#pragma once

#include <hz/node.h>
#include <hz/material.h>
#include <hz/program.h>

typedef struct _hz_mesh hz_mesh;

bool hz_mesh_set_geometry(hz_mesh *o,
		hz_vec3 *vertices, hz_vec3 *normals, size_t nvertices,
		hz_vec3 *uvs, size_t nuvs,
		unsigned *indices, size_t nindices);

hz_mesh *HZ_MESH(hz_node *o);
hz_node *hz_mesh_new(hz_arena *arena, hz_program *program, hz_material *m);
