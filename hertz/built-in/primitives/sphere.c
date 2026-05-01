#include <hz/built-in/primitives/sphere.h>
#include <hz/nodes/mesh.h>
#include <hz/program.h>
#include <hz/util/mem.h>
#include <math.h>

static inline void sphere_vertex(hz_vec3 *vertices, hz_vec3 *normals, hz_vec3 *uvs,
		int idx, float phi, float theta, int slices, int stack, int stacks)
{
	float x = sinf(phi) * cosf(theta);
	float y = cosf(phi);
	float z = sinf(phi) * sinf(theta);

	vertices[idx][0] = x;  vertices[idx][1] = y;  vertices[idx][2] = z;
	normals[idx][0]  = x;  normals[idx][1]  = y;  normals[idx][2]  = z;
	uvs[idx][0] = theta / (2.f * (float)M_PI);
	uvs[idx][1] = (float)stack / stacks;
	uvs[idx][2] = 0.f;
}

hz_node *hz_sphere_new(hz_arena *arena, hz_program *program, hz_material *material, unsigned quality)
{
	const int stacks = quality < 2 ? 2 : (int)quality;
	const int slices = stacks * 2;
	const int nvertices = stacks * slices * 6;

	hz_vec3 *vertices = hz_malloc(nvertices * sizeof(hz_vec3));
	hz_vec3 *normals  = hz_malloc(nvertices * sizeof(hz_vec3));
	hz_vec3 *uvs      = hz_malloc(nvertices * sizeof(hz_vec3));

	int k = 0;
	for (int i = 0; i < stacks; i++) {
		float phi0 = (float)M_PI * i / stacks;
		float phi1 = (float)M_PI * (i + 1) / stacks;

		for (int j = 0; j < slices; j++) {
			float theta0 = 2.f * (float)M_PI * j / slices;
			float theta1 = 2.f * (float)M_PI * (j + 1) / slices;

			/* triangle 1: v00, v10, v01 */
			sphere_vertex(vertices, normals, uvs, k++, phi0, theta0, slices, i,     stacks);
			sphere_vertex(vertices, normals, uvs, k++, phi1, theta0, slices, i + 1, stacks);
			sphere_vertex(vertices, normals, uvs, k++, phi0, theta1, slices, i,     stacks);

			/* triangle 2: v01, v10, v11 */
			sphere_vertex(vertices, normals, uvs, k++, phi0, theta1, slices, i,     stacks);
			sphere_vertex(vertices, normals, uvs, k++, phi1, theta0, slices, i + 1, stacks);
			sphere_vertex(vertices, normals, uvs, k++, phi1, theta1, slices, i + 1, stacks);
		}
	}

	hz_node *mesh = hz_mesh_new(arena, program, material);

	hz_mesh_set_geometry(HZ_MESH(mesh),
		vertices, normals, nvertices,
		uvs, nvertices,
		NULL, 0
	);

	hz_free(vertices);
	hz_free(normals);
	hz_free(uvs);

	return mesh;
}
