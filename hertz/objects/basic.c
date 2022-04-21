#include <string.h>
#include GL_EXTENSIONS_HEADER
#include <hz/helpers/shaders.h>
#include <hz/objects/basic.h>
#include <hz/camera.h>
#include <hz/logger.h>

static const char *tag = "bobj";

static inline void set_uniform_matrix(GLint uniform, mat4x4 value) {
	glUniformMatrix4fv(uniform, 1, GL_FALSE, (float *)value);
}

static bool find_uniforms(struct hz_basic_object *o)
{
	struct hz_object *super = hz_cast_object(o);

	o->uniforms.mvp = glGetUniformLocation(super->program, "MVP");
	if (o->uniforms.mvp < 0) {
		hz_log_e(tag, "unable to find MVP matrix in the given shader");
		return false;
	}

	o->uniforms.model = glGetUniformLocation(super->program, "model");
	if (o->uniforms.model < 0) {
		hz_log_e(tag, "unable to find model matrix in the given shader");
		return false;
	}

	return true;
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

	mat4x4_identity(o->transform.model);
	mat4x4_identity(o->transform.mvp);

	o->texture_attached = false;

	ok = find_uniforms(o);
	if (!ok) {
		hz_log_e(tag, "unable to find uniforms");
	}

	return ok;
}

bool hz_basic_object_set_texture(struct hz_basic_object *o, GLuint texture,
		vec3 *uvs, size_t uv_count)
{
	struct hz_object *super = hz_cast_object(o);

	o->buffers.uvs = hz_create_shader_attribute_buffer(super->program,
			"uv", 3, uvs, uv_count);
	if (!o->buffers.uvs) {
		hz_log_i(tag, "unable to create UV buffer");
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

	o->buffers.vertices = hz_create_shader_attribute_buffer(super->program,
			"position", 3, vertices, nvertices);
	if (!o->buffers.vertices) {
		hz_log_e(tag, "unable to create vertex buffer");
		goto fail;
	}

	o->buffers.indices = hz_create_gl_buffer(indices, sizeof(unsigned) * nindices);
	if (!o->buffers.indices) {
		hz_log_e(tag, "unable to create index buffer");
		goto fail_delete_vertices;
	}

	o->buffers.normals = hz_create_shader_attribute_buffer(super->program,
			"normal", 3, normals, nvertices);
	if (!o->buffers.normals) {
		hz_log_e(tag, "unable to create normal buffer");
		goto fail_delete_indices_and_vertices;
	}

	o->nindices = nindices;
	return true;

fail_delete_indices_and_vertices:
	glDeleteBuffers(1, &o->buffers.indices);
fail_delete_vertices:
	glDeleteBuffers(1, &o->buffers.vertices);
fail:
	return false;
}

const struct hz_object_proto hz_basic_object_proto = {
	.draw = basic_object_redraw,
	.init = basic_object_init,
	.deinit = basic_object_deinit,
};
