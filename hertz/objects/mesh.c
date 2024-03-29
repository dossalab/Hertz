#include HZ_GL_EXTENSIONS_HEADER
#include <hz/objects/mesh.h>
#include <hz/objects/camera.h>
#include <hz/material.h>
#include <vendor/linmath/linmath.h>
#include <hz/utils/container_of.h>
#include <internal/alloc.h>
#include <internal/object.h>
#include <internal/helpers/binders.h>

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

static inline void set_uniform_matrix(GLint uniform, mat4x4 value) {
	glUniformMatrix4fv(uniform, 1, GL_FALSE, (float *)value);
}

static void mesh_bind(struct hz_object *super)
{
	/* pass */
}

static void mesh_redraw(struct hz_object *super, struct hz_camera *c)
{
	struct hz_mesh *o = HZ_MESH(super);
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

bool hz_mesh_set_geometry(struct hz_mesh *o,
		vec3 *vertices, vec3 *normals, size_t nvertices,
		vec3 *uvs, size_t nuvs,
		unsigned *indices, size_t nindices)
{
	bool ok;

	struct hz_buffer_binding bindings[] = {
		{ &o->buffers.vertices, "position", 3, vertices, nvertices },
		{ &o->buffers.normals, "normal", 3, normals, nvertices },
		{ &o->buffers.uvs, "uv", 3, uvs, nuvs },
		{ &o->buffers.indices, NULL, 1, indices, nindices },
	};

	ok = hz_bind_buffers(bindings, o->program, HZ_ARRAY_SIZE(bindings));
	if (!ok) {
		return false;
	}

	o->nindices = nindices;
	return true;
}

const struct hz_object_proto hz_mesh_proto = {
	.bind = mesh_bind,
	.draw = mesh_redraw,
};

static bool hz_mesh_init(struct hz_mesh *o, GLuint program, struct hz_material *m)
{
	struct hz_object *super = HZ_OBJECT(o);
	bool ok;

	hz_object_init(super, &hz_mesh_proto);

	struct hz_uniform_binding bindings[] = {
		{ &o->uniforms.mvp, "MVP" },
		{ &o->uniforms.model, "model" },
	};

	ok = hz_bind_uniforms(bindings, program, HZ_ARRAY_SIZE(bindings));
	if (!ok) {
		return false;
	}

	o->material = m;
	o->program = program;

	glGenVertexArrays(1, &o->vao);
	glBindVertexArray(o->vao);

	return true;
}

void hz_mesh_deinit(struct hz_mesh *o)
{
	glDeleteBuffers(1, &o->buffers.vertices);
	glDeleteBuffers(1, &o->buffers.normals);
	glDeleteBuffers(1, &o->buffers.uvs);

	glDeleteVertexArrays(1, &o->vao);
}

struct hz_mesh *HZ_MESH(struct hz_object *o)
{
	return hz_container_of(o, struct hz_mesh, super);
}

struct hz_object *hz_mesh_new(GLuint program, struct hz_material *m)
{
	return HZ_OBJECT(hz_alloc_and_init(struct hz_mesh, hz_mesh_init, program, m));
}
