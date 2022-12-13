#include <hz/built-in/primitives/cube.h>
#include <hz/nodes/mesh.h>
#include <hz/util/common.h>

#define N_CUBE_VERTICES		HZ_ARRAY_SIZE(cube_vertices)
#define N_CUBE_INDICES		HZ_ARRAY_SIZE(cube_indices)

/* Blender default cube, imported by Assimp */
static hz_vec3 cube_vertices[] = {
	{  1.f,  1.f, -1.f },
	{ -1.f,  1.f, -1.f },
	{ -1.f,  1.f,  1.f },
	{  1.f,  1.f,  1.f },
	{  1.f, -1.f,  1.f },
	{  1.f,  1.f,  1.f },
	{ -1.f,  1.f,  1.f },
	{ -1.f, -1.f,  1.f },
	{ -1.f, -1.f,  1.f },
	{ -1.f,  1.f,  1.f },
	{ -1.f,  1.f, -1.f },
	{ -1.f, -1.f, -1.f },
	{ -1.f, -1.f, -1.f },
	{  1.f, -1.f, -1.f },
	{  1.f, -1.f,  1.f },
	{ -1.f, -1.f,  1.f },
	{  1.f, -1.f, -1.f },
	{  1.f,  1.f, -1.f },
	{  1.f,  1.f,  1.f },
	{  1.f, -1.f,  1.f },
	{ -1.f, -1.f, -1.f },
	{ -1.f,  1.f, -1.f },
	{  1.f,  1.f, -1.f },
	{  1.f, -1.f, -1.f },
};

static hz_vec3 cube_normals[N_CUBE_VERTICES] = {
	{  0.f,  1.f,  0.f },
	{  0.f,  1.f,  0.f },
	{  0.f,  1.f,  0.f },
	{  0.f,  1.f,  0.f },
	{  0.f,  0.f,  1.f },
	{  0.f,  0.f,  1.f },
	{  0.f,  0.f,  1.f },
	{  0.f,  0.f,  1.f },
	{ -1.f,  0.f,  0.f },
	{ -1.f,  0.f,  0.f },
	{ -1.f,  0.f,  0.f },
	{ -1.f,  0.f,  0.f },
	{  0.f, -1.f,  0.f },
	{  0.f, -1.f,  0.f },
	{  0.f, -1.f,  0.f },
	{  0.f, -1.f,  0.f },
	{  1.f,  0.f,  0.f },
	{  1.f,  0.f,  0.f },
	{  1.f,  0.f,  0.f },
	{  1.f,  0.f,  0.f },
	{  0.f,  0.f, -1.f },
	{  0.f,  0.f, -1.f },
	{  0.f,  0.f, -1.f },
	{  0.f,  0.f, -1.f },
};

static unsigned cube_indices[] = {
	0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9,
	10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16,
	17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23
};

hz_node *hz_cube_new(hz_arena *arena, int program, hz_material *material)
{
	hz_node *mesh = hz_mesh_new(arena, program, material);
	if (!mesh) {
		return NULL;
	}

	hz_mesh_set_geometry(HZ_MESH(mesh),
		cube_vertices, cube_normals,
		N_CUBE_VERTICES,
		NULL, 0,
		cube_indices, N_CUBE_INDICES
	);

	return mesh;
}
