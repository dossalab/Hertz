#include <GL/glew.h>
#include <time.h>
#include <string.h>

#include <counters/time.h>
#include <errors/errors.h>

#include "mesh.h"

static GLuint create_opengl_buffer(GLuint location, size_t components,
		void *data, size_t len)
{
	GLuint handle;
	const GLenum data_type = GL_FLOAT;
	const size_t byte_len = sizeof(float) * len * components;

	glGenBuffers(1, &handle);
	if (!handle) {
		return 0;
	}

	glBindBuffer(GL_ARRAY_BUFFER, handle);
	glBufferData(GL_ARRAY_BUFFER, byte_len, data, GL_STATIC_DRAW);

	glVertexAttribPointer(location, components, data_type, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(location);

	return handle;
}

static int mesh_create_geometry_buffers(struct mesh *m, vec3 *vertices,
		size_t vertex_count, vec3 *normals, size_t normal_count)
{
	int vb_location, nb_location;

	glGenVertexArrays(1, &m->vao);
	glBindVertexArray(m->vao);

	vb_location = glGetAttribLocation(m->program, "in_position");

	if (vb_location < 0) {
		return -ERR_SHADER_INVALID;
	}

	m->vbo = create_opengl_buffer(vb_location, 3, vertices, vertex_count);
	if (!m->vbo) {
		return -ERR_NO_VIDEO_BUFFER;
	}

	nb_location = glGetAttribLocation(m->program, "in_normal");
	if (nb_location < 0) {
		/* not an error - we can run like that if shader wants */
		return 0;
	}

	m->nbo = create_opengl_buffer(nb_location, 3, normals, normal_count);
	if (!m->nbo) {
		/* same - not an error (though we're out of video memory?) */
		return 0;
	}

	m->nbo_presented = true;
	return 0;
}

static int find_uniforms(struct mesh *m)
{
	m->mvp_handle = glGetUniformLocation(m->program, "MVP");
	if (m->mvp_handle < 0) {
		return -ERR_SHADER_INVALID;
	}

	m->model_handle = glGetUniformLocation(m->program, "model");
	m->model_presented = !(m->model_handle < 0);

	m->time_handle = glGetUniformLocation(m->program, "time");
	m->time_presented = !(m->time_handle < 0);

	return 0;
}

void mesh_update_mvp(struct mesh *m, mat4x4 vp)
{
	mat4x4_mul(m->mvp, vp, m->model);
}

void mesh_redraw(struct mesh *m)
{
	glBindVertexArray(m->vao);
	glUseProgram(m->program);

	/* so the previous one will be used (if exists) :) */
	if (m->texture_attached) {
		glBindTexture(GL_TEXTURE_2D, m->texture);
	}

	glUniformMatrix4fv(m->mvp_handle, 1, GL_FALSE, (float *)m->mvp);

	if (m->model_presented) {
		glUniformMatrix4fv(m->model_handle, 1, GL_FALSE, (float *)m->model);
	}

	if (m->time_presented) {
		glUniform1f(m->time_handle, global_time_counter);
	}

	glDrawArrays(GL_TRIANGLES, 0, m->vertex_count);
}

int mesh_texture(struct mesh *m, GLuint texture, vec2 *uvs, size_t uv_count)
{
	int tcb_location;

	tcb_location = glGetAttribLocation(m->program, "in_texcoords");
	if (tcb_location < 0) {
		return -ERR_SHADER_INVALID;
	}

	m->texture = texture;

	m->tbo = create_opengl_buffer(tcb_location, 2, uvs, uv_count);
	if (!m->tbo) {
		return -ERR_NO_VIDEO_BUFFER;
	}

	m->texture_attached = true;

	return 0;
}

int mesh_create_from_geometry(struct mesh *mesh, GLuint shader_program,
		vec3 *vertices, size_t vertex_count,
		vec3 *normals, size_t normal_count)
{
	int err;

	mat4x4_identity(mesh->model);
	mat4x4_identity(mesh->mvp);
	mesh->program = shader_program;
	mesh->vertex_count = vertex_count;

	mesh->texture_attached = false;
	mesh->nbo_presented = false;

	err = find_uniforms(mesh);
	if (err < 0) {
		return err;
	}

	return mesh_create_geometry_buffers(mesh, vertices, vertex_count,
			normals, normal_count);
}

void mesh_free(struct mesh *m)
{
	glDeleteBuffers(1, &m->vbo);

	if (m->nbo_presented) {
		glDeleteBuffers(1, &m->nbo);
	}

	if (m->texture_attached) {
		glDeleteBuffers(1, &m->tbo);
	}
}
