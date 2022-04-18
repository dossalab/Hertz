#include <GL/glew.h>
#include <string.h>
#include <utils/log.h>
#include <utils/gl/shaders.h>
#include "basic.h"

static const char *tag = "bobj";

static inline void set_uniform_matrix(GLint uniform, mat4x4 value) {
	glUniformMatrix4fv(uniform, 1, GL_FALSE, (float *)value);
}

static bool basic_object_create_geometry_buffers(struct basic_object *o,
		vec3 *vertices,
		vec3 *normals, size_t nvertices,
		unsigned *indices, size_t nindices)
{
	o->vbo = create_shader_attribute_buffer(o->program, "in_position",
			3, vertices, nvertices);
	if (!o->vbo) {
		log_e(tag, "unable to create vbo");
		return false;
	}

	o->ebo = create_gl_buffer(indices, sizeof(unsigned) * nindices);
	if (!o->ebo) {
		log_e(tag, "unable to create ebo");
	}

	o->nindices = nindices;

	o->nbo = create_shader_attribute_buffer(o->program, "in_normal",
			3, normals, nvertices);
	o->nbo_presented = o->nbo? true : false;

	return true;
}

static bool find_uniforms(struct basic_object *o)
{
	o->mvp_handle = glGetUniformLocation(o->program, "MVP");
	if (o->mvp_handle < 0) {
		log_e(tag, "no MVP handle");
		return false;
	}

	o->model_handle = glGetUniformLocation(o->program, "model");
	o->model_presented = !(o->model_handle < 0);

	o->time_handle = glGetUniformLocation(o->program, "time");
	o->time_presented = !(o->time_handle < 0);

	return true;
}

static void basic_object_update_mvp(struct object *_o, mat4x4 vp)
{
	struct basic_object *o = cast_basic_object(_o);

	mat4x4_mul(o->mvp, vp, o->model);
}

static void basic_object_redraw(struct object *_o)
{
	struct basic_object *o = cast_basic_object(_o);

	glUseProgram(o->program);

	/* so the previous one will be used (if exists) :) */
	if (o->texture_attached) {
		glBindTexture(GL_TEXTURE_2D, o->texture);
	}

	set_uniform_matrix(o->mvp_handle, o->mvp);

	if (o->model_presented) {
		set_uniform_matrix(o->model_handle, o->model);
	}

	if (o->time_presented) {
		// glUniform1f(o->time_handle, time);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, o->ebo);
	glDrawElements(GL_TRIANGLES, o->nindices, GL_UNSIGNED_INT, 0);
}

static void basic_object_free(struct object *_o)
{
	struct basic_object *o = cast_basic_object(_o);

	delete_gl_buffer(o->vbo);

	if (o->nbo_presented) {
		delete_gl_buffer(o->nbo);
	}

	if (o->texture_attached) {
		delete_gl_buffer(o->tbo);
	}
}

const struct object_proto basic_object_proto = {
	.draw = basic_object_redraw,
	.update_mvp = basic_object_update_mvp,
	.free = basic_object_free,
};

bool basic_object_texture(struct basic_object *o, GLuint texture, vec3 *uvs,
		size_t uv_count)
{
	o->texture = texture;

	o->tbo = create_shader_attribute_buffer(o->program, "in_texcoords",
			3, uvs, uv_count);
	if (!o->tbo) {
		log_i(tag, "no texture buffer basic_object");
		return false;
	}

	o->texture_attached = true;
	return true;
}

bool basic_object_create_from_geometry(struct basic_object *o,
		GLuint shader_program,
		vec3 *vertices, vec3 *normals, size_t nvertices,
		unsigned *indices, size_t nindices)
{
	bool ok;

	object_init(&o->as_object, &basic_object_proto);

	mat4x4_identity(o->model);
	mat4x4_identity(o->mvp);
	o->program = shader_program;

	o->texture_attached = false;
	o->nbo_presented = false;

	ok = find_uniforms(o);
	if (!ok) {
		log_e(tag, "unable to find uniforms");
		return false;
	}

	return basic_object_create_geometry_buffers(o, vertices, normals,
			nvertices, indices, nindices);
}

