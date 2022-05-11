#include GL_EXTENSIONS_HEADER
#include <hz/helpers/binders.h>
#include <hz/helpers/shaders.h>
#include <hz/objects/basic.h>
#include <hz/camera.h>
#include <vendor/linmath/linmath.h>

static inline void set_uniform_matrix(GLint uniform, mat4x4 value) {
	glUniformMatrix4fv(uniform, 1, GL_FALSE, (float *)value);
}

static void basic_object_redraw(struct hz_object *super, struct hz_camera *c)
{
	struct hz_basic_object *o = hz_cast_basic_object(super);

	glBindVertexArray(o->vao);
	glUseProgram(o->program);

	hz_material_use(o->material);

	mat4x4_mul(o->transform.mvp, c->vp, o->transform.model);

	set_uniform_matrix(o->uniforms.mvp, o->transform.mvp);
	set_uniform_matrix(o->uniforms.model, o->transform.model);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, o->buffers.indices);
	glDrawElements(GL_TRIANGLES, o->nindices, GL_UNSIGNED_INT, 0);
}

static void basic_object_deinit(struct hz_object *super)
{
	struct hz_basic_object *o = hz_cast_basic_object(super);

	hz_delete_gl_buffer(o->buffers.vertices);
	hz_delete_gl_buffer(o->buffers.normals);
	hz_delete_gl_buffer(o->buffers.uvs);

	glDeleteVertexArrays(1, &o->vao);
}


bool hz_basic_object_set_geometry(struct hz_basic_object *o,
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

const struct hz_object_proto hz_basic_object_proto = {
	.draw = basic_object_redraw,
	.deinit = basic_object_deinit,
};

bool hz_basic_object_init(struct hz_basic_object *o, GLuint program,
		struct hz_material *m)
{
	struct hz_object *super = hz_cast_object(o);
	bool ok;

	hz_object_set_proto(super, &hz_basic_object_proto);

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

	mat4x4_identity(o->transform.model);
	mat4x4_identity(o->transform.mvp);

	return true;
}
