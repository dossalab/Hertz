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

	if (o->texture_attached) {
		glBindTexture(GL_TEXTURE_2D, o->texture);
	}

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

	if (o->texture_attached) {
		hz_delete_gl_buffer(o->buffers.uvs);
	}
}

static bool basic_object_init(struct hz_object *super)
{
	struct hz_basic_object *o = hz_cast_basic_object(super);
	bool ok;

	struct hz_uniform_binding bindings[] = {
		{ &o->uniforms.mvp, "MVP" },
		{ &o->uniforms.model, "model" },
	};

	ok = hz_bind_uniforms(bindings, super->program, HZ_ARRAY_SIZE(bindings));
	if (!ok) {
		return false;
	}

	mat4x4_identity(o->transform.model);
	mat4x4_identity(o->transform.mvp);

	o->texture_attached = false;

	return true;
}

bool hz_basic_object_set_texture(struct hz_basic_object *o, GLuint texture,
		vec3 *uvs, size_t uv_count)
{
	struct hz_object *super = hz_cast_object(o);
	bool ok;

	struct hz_buffer_binding bindings[] = {
		{ &o->buffers.uvs, "uv", 3, uvs, uv_count },
	};

	ok = hz_bind_buffers(bindings, super->program, HZ_ARRAY_SIZE(bindings));
	if (!ok) {
		return false;
	}

	o->texture_attached = true;
	o->texture = texture;

	return true;
}

bool hz_basic_object_set_geometry(struct hz_basic_object *o,
		vec3 *vertices, vec3 *normals, size_t nvertices,
		unsigned *indices, size_t nindices)
{
	struct hz_object *super = hz_cast_object(o);
	bool ok;

	struct hz_buffer_binding bindings[] = {
		{ &o->buffers.vertices, "position", 3, vertices, nvertices },
		{ &o->buffers.normals, "normal", 3, normals, nvertices },
		{ &o->buffers.indices, NULL, 1, indices, nindices },
	};

	ok = hz_bind_buffers(bindings, super->program, HZ_ARRAY_SIZE(bindings));
	if (!ok) {
		return false;
	}

	o->nindices = nindices;
	return true;
}

const struct hz_object_proto hz_basic_object_proto = {
	.draw = basic_object_redraw,
	.init = basic_object_init,
	.deinit = basic_object_deinit,
};
