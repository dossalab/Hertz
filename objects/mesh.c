#include <GL/glew.h>
#include <string.h>
#include <logger/logger.h>

#include <utils/gl/shaders.h>

#include "mesh.h"

static bool mesh_create_geometry_buffers(struct mesh *m, vec3 *vertices,
		size_t vertex_count, vec3 *normals, size_t normal_count)
{
	glGenVertexArrays(1, &m->vao);
	glBindVertexArray(m->vao);

	m->vbo = create_shader_attribute_buffer(m->program, "in_position",
			3, vertices, vertex_count);
	if (!m->vbo) {
		log_e("unable to create vbo");
		return false;
	}

	m->nbo = create_shader_attribute_buffer(m->program, "in_normal",
			3, normals, normal_count);
	m->nbo_presented = m->nbo? true : false;

	return true;
}

static bool find_uniforms(struct mesh *m)
{
	m->mvp_handle = get_shader_uniform_handle(m->program, "MVP");
	if (m->mvp_handle < 0) {
		return false;
	}

	m->model_handle = get_shader_uniform_handle(m->program, "model");
	m->model_presented = !(m->model_handle < 0);

	m->time_handle = get_shader_uniform_handle(m->program, "time");
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
	use_shader_program(m->program);

	/* so the previous one will be used (if exists) :) */
	if (m->texture_attached) {
		glBindTexture(GL_TEXTURE_2D, m->texture);
	}

	set_shader_uniform_matrix(m->mvp_handle, m->mvp);

	if (m->model_presented) {
		set_shader_uniform_matrix(m->model_handle, m->model);
	}

	if (m->time_presented) {
		set_shader_uniform_float(m->time_handle, time);
	}

	glDrawArrays(GL_TRIANGLES, 0, m->vertex_count);
}

bool mesh_texture(struct mesh *m, GLuint texture, vec2 *uvs, size_t uv_count)
{
	m->texture = texture;

	m->tbo = create_shader_attribute_buffer(m->program, "in_texcoords",
			2, uvs, uv_count);
	if (!m->tbo) {
		log_i("no texture buffer object");
		return false;
	}

	m->texture_attached = true;
	return true;
}

bool mesh_create_from_geometry(struct mesh *mesh, GLuint shader_program,
		vec3 *vertices, size_t vertex_count,
		vec3 *normals, size_t normal_count)
{
	bool ok;

	mat4x4_identity(mesh->model);
	mat4x4_identity(mesh->mvp);
	mesh->program = shader_program;
	mesh->vertex_count = vertex_count;

	mesh->texture_attached = false;
	mesh->nbo_presented = false;

	ok = find_uniforms(mesh);
	if (!ok) {
		log_e("unable to find uniforms");
		return false;
	}

	return mesh_create_geometry_buffers(mesh, vertices, vertex_count,
			normals, normal_count);
}

void mesh_free(struct mesh *m)
{
	delete_shader_attribute_buffer(m->vbo);

	if (m->nbo_presented) {
		delete_shader_attribute_buffer(m->nbo);
	}

	if (m->texture_attached) {
		delete_shader_attribute_buffer(m->tbo);
	}
}