#include HZ_GL_EXTENSIONS_HEADER
#include <string.h>
#include <hz/nodes/mesh.h>
#include <hz/material.h>
#include <hz/internal/program.h>
#include <vendor/linmath/linmath.h>
#include <hz/internal/node.h>
#include <hz/internal/helpers/binders.h>

struct _hz_mesh {
	hz_node super;
	GLuint vao;
	hz_program *program;

	struct {
		GLuint vertices, normals, uvs, barycentrics, indices;
	} buffers;

	struct {
		GLint mvp, model, camera_pos;
	} uniforms;

	hz_material *material;
	size_t ndraw;
	bool indexed;
};

static inline void set_uniform_matrix(GLint uniform, mat4x4 value) {
	glUniformMatrix4fv(uniform, 1, GL_FALSE, (float *)value);
}

static void mesh_draw(hz_node *super, hz_camera *c)
{
	hz_mesh *o = HZ_MESH(super);
	mat4x4 vp, mvp;
	vec3 cam_pos;

	glBindVertexArray(o->vao);
	glUseProgram(o->program->id);

	hz_material_use(o->material);

	hz_camera_get_vp(c, vp);
	hz_camera_get_position(c, cam_pos);
	mat4x4_mul(mvp, vp, super->model);

	set_uniform_matrix(o->uniforms.mvp, mvp);
	set_uniform_matrix(o->uniforms.model, super->model);
	glUniform3fv(o->uniforms.camera_pos, 1, cam_pos);

	if (o->indexed) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, o->buffers.indices);
		glDrawElements(GL_TRIANGLES, o->ndraw, GL_UNSIGNED_INT, 0);
	} else {
		glDrawArrays(GL_TRIANGLES, 0, o->ndraw);
	}
}

static void generate_barycentrics(hz_vec3 *bary, size_t nvertices)
{
	static const float pattern[3][3] = {
		{ 1.f, 0.f, 0.f },
		{ 0.f, 1.f, 0.f },
		{ 0.f, 0.f, 1.f },
	};

	for (size_t i = 0; i < nvertices; i++) {
		memcpy(bary[i], pattern[i % 3], sizeof(hz_vec3));
	}
}

bool hz_mesh_set_geometry(hz_mesh *n,
		vec3 *vertices, vec3 *normals, size_t nvertices,
		vec3 *uvs, size_t nuvs,
		unsigned *indices, size_t nindices)
{
	bool ok;

	struct hz_buffer_binding bindings[] = {
		{ &n->buffers.vertices, "position", 3, vertices, nvertices },
		{ &n->buffers.normals, "normal", 3, normals, nvertices },
	};

	ok = hz_bind_buffers(bindings, n->program->id, HZ_ARRAY_SIZE(bindings));
	if (!ok) {
		return false;
	}

	if (uvs && nuvs) {
		struct hz_buffer_binding uv_binding =
			{ &n->buffers.uvs, "uv", 3, uvs, nuvs };
		hz_bind_buffers(&uv_binding, n->program->id, 1);
	}

	if (indices && nindices) {
		struct hz_buffer_binding idx_binding =
			{ &n->buffers.indices, NULL, 1, indices, nindices };
		ok = hz_bind_buffers(&idx_binding, n->program->id, 1);
		if (!ok) {
			return false;
		}
		n->indexed = true;
		n->ndraw = nindices;
	} else {
		hz_vec3 bary[nvertices];

		generate_barycentrics(bary, nvertices);

		struct hz_buffer_binding bary_binding =
			{ &n->buffers.barycentrics, "barycentric", 3, bary, nvertices };
		hz_bind_buffers(&bary_binding, n->program->id, 1);

		n->indexed = false;
		n->ndraw = nvertices;
	}

	return true;
}

hz_mesh *HZ_MESH(hz_node *n)
{
	return hz_container_of(n, hz_mesh, super);
}

static void mesh_init(hz_node *super, hz_program *program, hz_material *m)
{
	hz_mesh *mesh = HZ_MESH(super);

	struct hz_uniform_binding bindings[] = {
		{ &mesh->uniforms.mvp, "MVP" },
		{ &mesh->uniforms.model, "model" },
		{ &mesh->uniforms.camera_pos, "camera" },
	};

	hz_bind_uniforms(bindings, program->id, HZ_ARRAY_SIZE(bindings));

	mesh->material = m;
	mesh->program = program;
	memset(&mesh->buffers, 0, sizeof(mesh->buffers));

	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);
}

static void mesh_free(void *super)
{
	hz_mesh *mesh = HZ_MESH(super);

	glDeleteBuffers(1, &mesh->buffers.vertices);
	glDeleteBuffers(1, &mesh->buffers.normals);
	glDeleteBuffers(1, &mesh->buffers.uvs);
	glDeleteBuffers(1, &mesh->buffers.barycentrics);
	glDeleteBuffers(1, &mesh->buffers.indices);

	glDeleteVertexArrays(1, &mesh->vao);
}

const hz_node_proto hz_mesh_proto = {
	.bind = hz_node_dummy_bind,
	.draw = mesh_draw,

	.arena_proto = {
		.name = "meshes",
		.free = mesh_free,
		.size = sizeof(hz_mesh),
	}
};

hz_node *hz_mesh_new(hz_arena *arena, hz_program *program, hz_material *m)
{
	hz_node *super = hz_node_new(arena, hz_mesh, &hz_mesh_proto);
	mesh_init(super, program, m);

	return super;
}
