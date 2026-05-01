#include <hz/built-in/primitives/cube.h>
#include <hz/nodes/mesh.h>
#include <hz/program.h>
#include <hz/util/common.h>

#define N_CUBE_VERTICES		HZ_ARRAY_SIZE(cube_vertices)

/* Blender default cube, expanded to unindexed triangles (no shared vertices).
 * Each face is two triangles; vertices are listed as: a,b,c, a,c,d */
static hz_vec3 cube_vertices[] = {
	/* top (y=1) */
	{  1.f,  1.f, -1.f }, { -1.f,  1.f, -1.f }, { -1.f,  1.f,  1.f },
	{  1.f,  1.f, -1.f }, { -1.f,  1.f,  1.f }, {  1.f,  1.f,  1.f },
	/* front (z=1) */
	{  1.f, -1.f,  1.f }, {  1.f,  1.f,  1.f }, { -1.f,  1.f,  1.f },
	{  1.f, -1.f,  1.f }, { -1.f,  1.f,  1.f }, { -1.f, -1.f,  1.f },
	/* left (x=-1) */
	{ -1.f, -1.f,  1.f }, { -1.f,  1.f,  1.f }, { -1.f,  1.f, -1.f },
	{ -1.f, -1.f,  1.f }, { -1.f,  1.f, -1.f }, { -1.f, -1.f, -1.f },
	/* bottom (y=-1) */
	{ -1.f, -1.f, -1.f }, {  1.f, -1.f, -1.f }, {  1.f, -1.f,  1.f },
	{ -1.f, -1.f, -1.f }, {  1.f, -1.f,  1.f }, { -1.f, -1.f,  1.f },
	/* right (x=1) */
	{  1.f, -1.f, -1.f }, {  1.f,  1.f, -1.f }, {  1.f,  1.f,  1.f },
	{  1.f, -1.f, -1.f }, {  1.f,  1.f,  1.f }, {  1.f, -1.f,  1.f },
	/* back (z=-1) */
	{ -1.f, -1.f, -1.f }, { -1.f,  1.f, -1.f }, {  1.f,  1.f, -1.f },
	{ -1.f, -1.f, -1.f }, {  1.f,  1.f, -1.f }, {  1.f, -1.f, -1.f },
};

static hz_vec3 cube_normals[N_CUBE_VERTICES] = {
	{  0.f,  1.f,  0.f }, {  0.f,  1.f,  0.f }, {  0.f,  1.f,  0.f },
	{  0.f,  1.f,  0.f }, {  0.f,  1.f,  0.f }, {  0.f,  1.f,  0.f },
	{  0.f,  0.f,  1.f }, {  0.f,  0.f,  1.f }, {  0.f,  0.f,  1.f },
	{  0.f,  0.f,  1.f }, {  0.f,  0.f,  1.f }, {  0.f,  0.f,  1.f },
	{ -1.f,  0.f,  0.f }, { -1.f,  0.f,  0.f }, { -1.f,  0.f,  0.f },
	{ -1.f,  0.f,  0.f }, { -1.f,  0.f,  0.f }, { -1.f,  0.f,  0.f },
	{  0.f, -1.f,  0.f }, {  0.f, -1.f,  0.f }, {  0.f, -1.f,  0.f },
	{  0.f, -1.f,  0.f }, {  0.f, -1.f,  0.f }, {  0.f, -1.f,  0.f },
	{  1.f,  0.f,  0.f }, {  1.f,  0.f,  0.f }, {  1.f,  0.f,  0.f },
	{  1.f,  0.f,  0.f }, {  1.f,  0.f,  0.f }, {  1.f,  0.f,  0.f },
	{  0.f,  0.f, -1.f }, {  0.f,  0.f, -1.f }, {  0.f,  0.f, -1.f },
	{  0.f,  0.f, -1.f }, {  0.f,  0.f, -1.f }, {  0.f,  0.f, -1.f },
};

/* UVs follow the (0,0),(1,0),(1,1) and (0,0),(1,1),(0,1) split per face */
static hz_vec3 cube_uvs[N_CUBE_VERTICES] = {
	{ 0.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, { 1.f, 1.f, 0.f },
	{ 0.f, 0.f, 0.f }, { 1.f, 1.f, 0.f }, { 0.f, 1.f, 0.f },
	{ 0.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, { 1.f, 1.f, 0.f },
	{ 0.f, 0.f, 0.f }, { 1.f, 1.f, 0.f }, { 0.f, 1.f, 0.f },
	{ 0.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, { 1.f, 1.f, 0.f },
	{ 0.f, 0.f, 0.f }, { 1.f, 1.f, 0.f }, { 0.f, 1.f, 0.f },
	{ 0.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, { 1.f, 1.f, 0.f },
	{ 0.f, 0.f, 0.f }, { 1.f, 1.f, 0.f }, { 0.f, 1.f, 0.f },
	{ 0.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, { 1.f, 1.f, 0.f },
	{ 0.f, 0.f, 0.f }, { 1.f, 1.f, 0.f }, { 0.f, 1.f, 0.f },
	{ 0.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, { 1.f, 1.f, 0.f },
	{ 0.f, 0.f, 0.f }, { 1.f, 1.f, 0.f }, { 0.f, 1.f, 0.f },
};

hz_node *hz_cube_new(hz_arena *arena, hz_program *program, hz_material *material)
{
	hz_node *mesh = hz_mesh_new(arena, program, material);

	hz_mesh_set_geometry(HZ_MESH(mesh),
		cube_vertices, cube_normals,
		N_CUBE_VERTICES,
		cube_uvs, N_CUBE_VERTICES,
		NULL, 0
	);

	return mesh;
}
