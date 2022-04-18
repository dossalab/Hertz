#include <GL/glew.h>
#include <string.h>
#include <utils/log.h>
#include <utils/gl/shaders.h>
#include "basic.h"

static const char *tag = "bobj";

static inline void set_uniform_matrix(GLint uniform, mat4x4 value) {
	glUniformMatrix4fv(uniform, 1, GL_FALSE, (float *)value);
}

static bool find_uniforms(struct basic_object *o)
{
	o->uniforms.mvp = glGetUniformLocation(o->as_object.program, "MVP");
	if (o->uniforms.mvp < 0) {
		log_e(tag, "unable to find MVP matrix in the given shader");
		return false;
	}

	o->uniforms.model = glGetUniformLocation(o->as_object.program, "model");
	if (o->uniforms.model < 0) {
		log_e(tag, "unable to find model matrix in the given shader");
		return false;
	}

	return true;
}

static void basic_object_update_mvp(struct object *_o, mat4x4 vp)
{
	struct basic_object *o = cast_basic_object(_o);

	mat4x4_mul(o->transform.mvp, vp, o->transform.model);
}

static void basic_object_redraw(struct object *_o)
{
	struct basic_object *o = cast_basic_object(_o);

	if (o->texture_attached) {
		glBindTexture(GL_TEXTURE_2D, o->texture);
	}

	set_uniform_matrix(o->uniforms.mvp, o->transform.mvp);
	set_uniform_matrix(o->uniforms.model, o->transform.model);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, o->buffers.indices);
	glDrawElements(GL_TRIANGLES, o->nindices, GL_UNSIGNED_INT, 0);
}

static void basic_object_deinit(struct object *_o)
{
	struct basic_object *o = cast_basic_object(_o);

	delete_gl_buffer(o->buffers.vertices);
	delete_gl_buffer(o->buffers.normals);

	if (o->texture_attached) {
		delete_gl_buffer(o->buffers.uvs);
	}
}

static bool basic_object_init(struct object *_o)
{
	struct basic_object *o = cast_basic_object(_o);
	bool ok;

	mat4x4_identity(o->transform.model);
	mat4x4_identity(o->transform.mvp);

	o->texture_attached = false;

	ok = find_uniforms(o);
	if (!ok) {
		log_e(tag, "unable to find uniforms");
	}

	return ok;
}

bool basic_object_set_texture(struct basic_object *o, GLuint texture,
		vec3 *uvs, size_t uv_count)
{
	o->buffers.uvs = create_shader_attribute_buffer(o->as_object.program,
			"in_texcoords", 3, uvs, uv_count);
	if (!o->buffers.uvs) {
		log_i(tag, "unable to create UV buffer");
		return false;
	}

	o->texture_attached = true;
	o->texture = texture;

	return true;
}

bool basic_object_set_geometry(struct basic_object *o,
		vec3 *vertices, vec3 *normals, size_t nvertices,
		unsigned *indices, size_t nindices)
{
	o->buffers.vertices = create_shader_attribute_buffer(o->as_object.program,
			"in_position", 3, vertices, nvertices);
	if (!o->buffers.vertices) {
		log_e(tag, "unable to create vertex buffer");
		goto fail;
	}

	o->buffers.indices = create_gl_buffer(indices, sizeof(unsigned) * nindices);
	if (!o->buffers.indices) {
		log_e(tag, "unable to create index buffer");
		goto fail_delete_vertices;
	}

	o->buffers.normals = create_shader_attribute_buffer(o->as_object.program,
			"in_normal", 3, normals, nvertices);
	if (!o->buffers.normals) {
		log_e(tag, "unable to create normal buffer");
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

const struct object_proto basic_object_proto = {
	.draw = basic_object_redraw,
	.update_mvp = basic_object_update_mvp,
	.init = basic_object_init,
	.deinit = basic_object_deinit,
};
