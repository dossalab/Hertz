#include <GL/glew.h>
#include <string.h>
#include <utils/log.h>

#include <utils/gl/shaders.h>

#include "mesh.h"

static const char *tag = "mesh";

static inline void set_uniform_matrix(GLint uniform, mat4x4 value) {
	glUniformMatrix4fv(uniform, 1, GL_FALSE, (float *)value);
}

static bool mesh_create_geometry_buffers(struct mesh *m, vec3 *vertices,
		vec3 *normals, size_t nvertices,
		unsigned *indices, size_t nindices)
{
	glGenVertexArrays(1, &m->vao);
	glBindVertexArray(m->vao);

	m->vbo = create_shader_attribute_buffer(m->program, "in_position",
			3, vertices, nvertices);
	if (!m->vbo) {
		log_e(tag, "unable to create vbo");
		return false;
	}

	m->ebo = create_gl_buffer(indices, sizeof(unsigned) * nindices);
	if (!m->ebo) {
		log_e(tag, "unable to create ebo");
	}

	m->nindices = nindices;

	m->nbo = create_shader_attribute_buffer(m->program, "in_normal",
			3, normals, nvertices);
	m->nbo_presented = m->nbo? true : false;

	return true;
}

static bool find_uniforms(struct mesh *m)
{
	m->mvp_handle = glGetUniformLocation(m->program, "MVP");
	if (m->mvp_handle < 0) {
		log_e(tag, "no MVP handle");
		return false;
	}

	m->model_handle = glGetUniformLocation(m->program, "model");
	m->model_presented = !(m->model_handle < 0);

	m->time_handle = glGetUniformLocation(m->program, "time");
	m->time_presented = !(m->time_handle < 0);

	return true;
}

void mesh_update_mvp(struct mesh *m, mat4x4 vp)
{
	mat4x4_mul(m->mvp, vp, m->model);
}

void mesh_redraw(struct mesh *m, float time)
{
	glBindVertexArray(m->vao);
	glUseProgram(m->program);

	/* so the previous one will be used (if exists) :) */
	if (m->texture_attached) {
		glBindTexture(GL_TEXTURE_2D, m->texture);
	}

	set_uniform_matrix(m->mvp_handle, m->mvp);

	if (m->model_presented) {
		set_uniform_matrix(m->model_handle, m->model);
	}

	if (m->time_presented) {
		glUniform1f(m->time_handle, time);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ebo);
	glDrawElements(GL_TRIANGLES, m->nindices, GL_UNSIGNED_INT, 0);
}

bool mesh_texture(struct mesh *m, GLuint texture, vec3 *uvs, size_t uv_count)
{
	m->texture = texture;

	m->tbo = create_shader_attribute_buffer(m->program, "in_texcoords",
			3, uvs, uv_count);
	if (!m->tbo) {
		log_i(tag, "no texture buffer object");
		return false;
	}

	m->texture_attached = true;
	return true;
}

bool mesh_create_from_geometry(struct mesh *mesh, GLuint shader_program,
		vec3 *vertices, vec3 *normals, size_t nvertices,
		unsigned *indices, size_t nindices)
{
	bool ok;

	mat4x4_identity(mesh->model);
	mat4x4_identity(mesh->mvp);
	mesh->program = shader_program;

	mesh->texture_attached = false;
	mesh->nbo_presented = false;

	ok = find_uniforms(mesh);
	if (!ok) {
		log_e(tag, "unable to find uniforms");
		return false;
	}

	return mesh_create_geometry_buffers(mesh, vertices, normals,
			nvertices, indices, nindices);
}

void mesh_free(struct mesh *m)
{
	delete_gl_buffer(m->vbo);

	if (m->nbo_presented) {
		delete_gl_buffer(m->nbo);
	}

	if (m->texture_attached) {
		delete_gl_buffer(m->tbo);
	}
}
