#include HZ_GL_EXTENSIONS_HEADER
#include <hz/nodes/mesh.h>
#include <hz/material.h>
#include <vendor/linmath/linmath.h>
#include <hz/internal/node.h>
#include <hz/internal/helpers/binders.h>

struct _hz_mesh {
	hz_node super;
	GLuint vao;
	GLuint program;

	struct {
		GLuint vertices, normals, uvs, indices;
	} buffers;

	struct {
		GLint mvp, model;
	} uniforms;

	hz_material *material;
	size_t nindices;
};

static inline void set_uniform_matrix(GLint uniform, mat4x4 value) {
	glUniformMatrix4fv(uniform, 1, GL_FALSE, (float *)value);
}

static void mesh_draw(hz_node *super, hz_camera *c)
{
	hz_mesh *o = HZ_MESH(super);
	mat4x4 vp, mvp;

	glBindVertexArray(o->vao);
	glUseProgram(o->program);

	hz_material_use(o->material);

	hz_camera_get_vp(c, vp);
	mat4x4_mul(mvp, vp, super->model);

	set_uniform_matrix(o->uniforms.mvp, mvp);
	set_uniform_matrix(o->uniforms.model, super->model);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, o->buffers.indices);
	glDrawElements(GL_TRIANGLES, o->nindices, GL_UNSIGNED_INT, 0);
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
		{ &n->buffers.uvs, "uv", 3, uvs, nuvs },
		{ &n->buffers.indices, NULL, 1, indices, nindices },
	};

	ok = hz_bind_buffers(bindings, n->program, HZ_ARRAY_SIZE(bindings));
	if (!ok) {
		return false;
	}

	n->nindices = nindices;
	return true;
}

hz_mesh *HZ_MESH(hz_node *n)
{
	return hz_container_of(n, hz_mesh, super);
}

static void mesh_init(hz_node *super, GLuint program, hz_material *m)
{
	bool ok;
	hz_mesh *mesh = HZ_MESH(super);

	struct hz_uniform_binding bindings[] = {
		{ &mesh->uniforms.mvp, "MVP" },
		{ &mesh->uniforms.model, "model" },
	};

	ok = hz_bind_uniforms(bindings, program, HZ_ARRAY_SIZE(bindings));
	if (!ok) {
		return;
	}

	mesh->material = m;
	mesh->program = program;

	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);
}

static void mesh_free(void *super)
{
	hz_mesh *mesh = HZ_MESH(super);

	glDeleteBuffers(1, &mesh->buffers.vertices);
	glDeleteBuffers(1, &mesh->buffers.normals);
	glDeleteBuffers(1, &mesh->buffers.uvs);

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

hz_node *hz_mesh_new(hz_arena *arena, GLuint program, hz_material *m)
{
	hz_node *super = hz_node_new(arena, hz_mesh, &hz_mesh_proto);
	if (!super) {
		return NULL;
	}

	mesh_init(super, program, m);

	return super;
}
