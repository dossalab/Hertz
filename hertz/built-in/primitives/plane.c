#include <hz/built-in/primitives/plane.h>
#include <hz/nodes/mesh.h>
#include <hz/util/common.h>

/* Unit plane in the XZ plane (y=0), two unindexed triangles */
static hz_vec3 plane_vertices[] = {
	{ -1.f, 0.f, -1.f }, { -1.f, 0.f,  1.f }, {  1.f, 0.f,  1.f },
	{ -1.f, 0.f, -1.f }, {  1.f, 0.f,  1.f }, {  1.f, 0.f, -1.f },
};

#define N_PLANE_VERTICES	HZ_ARRAY_SIZE(plane_vertices)

static hz_vec3 plane_normals[N_PLANE_VERTICES] = {
	{ 0.f, 1.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 1.f, 0.f },
	{ 0.f, 1.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 1.f, 0.f },
};

static hz_vec3 plane_uvs[N_PLANE_VERTICES] = {
	{ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, { 1.f, 1.f, 0.f },
	{ 0.f, 0.f, 0.f }, { 1.f, 1.f, 0.f }, { 1.f, 0.f, 0.f },
};

hz_node *hz_plane_new(hz_arena *arena, hz_program *program, hz_material *material)
{
	hz_node *mesh = hz_mesh_new(arena, program, material);

	hz_mesh_set_geometry(HZ_MESH(mesh),
		plane_vertices, plane_normals,
		N_PLANE_VERTICES,
		plane_uvs, N_PLANE_VERTICES,
		NULL, 0
	);

	return mesh;
}
