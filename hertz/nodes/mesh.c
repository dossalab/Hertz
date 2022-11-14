#include HZ_GL_EXTENSIONS_HEADER
#include <hz/nodes/mesh.h>
#include <hz/material.h>
#include <vendor/linmath/linmath.h>
#include <hz/internal/alloc.h>
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

static void mesh_bind(hz_node *super)
{
	/* pass */
}

static void mesh_redraw(hz_node *super, hz_camera *c)
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

const hz_node_proto hz_mesh_proto = {
	.bind = mesh_bind,
	.draw = mesh_redraw,
};

static bool hz_mesh_init(hz_mesh *n, GLuint program, hz_material *m)
{
	hz_node *super = HZ_NODE(n);
	bool ok;

	hz_node_init(super, &hz_mesh_proto);

	struct hz_uniform_binding bindings[] = {
		{ &n->uniforms.mvp, "MVP" },
		{ &n->uniforms.model, "model" },
	};

	ok = hz_bind_uniforms(bindings, program, HZ_ARRAY_SIZE(bindings));
	if (!ok) {
		return false;
	}

	n->material = m;
	n->program = program;

	glGenVertexArrays(1, &n->vao);
	glBindVertexArray(n->vao);

	return true;
}

void hz_mesh_deinit(hz_mesh *n)
{
	glDeleteBuffers(1, &n->buffers.vertices);
	glDeleteBuffers(1, &n->buffers.normals);
	glDeleteBuffers(1, &n->buffers.uvs);

	glDeleteVertexArrays(1, &n->vao);
}

hz_mesh *HZ_MESH(hz_node *n)
{
	return hz_container_of(n, hz_mesh, super);
}

hz_node *hz_mesh_new(GLuint program, hz_material *m)
{
	return HZ_NODE(hz_alloc_and_init(hz_mesh, hz_mesh_init, program, m));
}
