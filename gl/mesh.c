#include <GL/glew.h>
#include <time.h>
#include <string.h>

#include <app/time.h>
#include <logger/logger.h>
#include <errors/errors.h>
#include <gl/textures.h>
#include <loaders/obj_loader.h>
#include <loaders/stb_image.h>

#include "mesh.h"

static GLuint create_opengl_buffer(GLuint location, size_t components,
		void *data, size_t len)
{
	GLuint handle;

	glGenBuffers(1, &handle);
	if (!handle) {
		return 0;
	}

	glBindBuffer(GL_ARRAY_BUFFER, handle);
	glBufferData(GL_ARRAY_BUFFER, len, data, GL_STATIC_DRAW);

	glVertexAttribPointer(location, components, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(location);

	return handle;
}

static int mesh_create_buffers(struct mesh *m, struct model *model)
{
	int vb_location, nb_location;

	glGenVertexArrays(1, &m->vao);
	glBindVertexArray(m->vao);

	vb_location = glGetAttribLocation(m->program, "in_position");

	if (vb_location < 0) {
		return -ERR_SHADER_INVALID;
	}

	m->vbo = create_opengl_buffer(vb_location, 3, model->vertices,
			model->vertex_count * sizeof(struct vertex));
	if (!m->vbo) {
		return -ERR_NO_VIDEO_BUFFER;
	}

	nb_location = glGetAttribLocation(m->program, "in_normal");
	if (nb_location < 0) {
		log_d("shader doesn't require normal buffer");

		/* not an error - we can run like that if shader wants */
		return 0;
	}

	m->nbo = create_opengl_buffer(nb_location, 3, model->normals,
			model->normal_count * sizeof(struct vertex));
	if (!m->nbo) {
		glDeleteBuffers(1, &m->vbo);
		return -ERR_NO_VIDEO_BUFFER;
	}

	return 0;
}

static GLuint create_texture_from_file(const char *path)
{
	int w, h, n;
	uint8_t *data;
	GLuint texture;

	data = stbi_load(path, &w, &h, &n, 3);
	if (!data) {
		return 0;
	}

	texture = create_texture_from_rgb(data, w, h);
	stbi_image_free(data);

	return texture;
}

static int mesh_attach_textures(struct mesh *m, struct model *model)
{
	int tcb_location;
	GLuint texture;

	tcb_location = glGetAttribLocation(m->program, "in_texcoords");
	if (tcb_location < 0) {
		return -ERR_SHADER_INVALID;
	}

	texture = create_texture_from_file(model->texture_path);
	if (!texture) {
		return -ERR_NO_FILE;
	}

	m->texture = texture;
	m->tbo = create_opengl_buffer(tcb_location, 2, model->uvs, 
			model->uv_count * sizeof(struct point));

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

	if (!m->model_presented) {
		log_d("no model matrix in the shader");
	}

	m->time_handle = glGetUniformLocation(m->program, "time");
	m->time_presented = !(m->time_handle < 0);

	return 0;
}

static int mesh_load_from_model(struct mesh *mesh, GLuint shader_program,
		struct model *model)
{
	int err;

	mat4x4_identity(mesh->model);
	mesh->program = shader_program;
	mesh->vertex_count = model->vertex_count;

	err = find_uniforms(mesh);
	if (err < 0) {
		return err;
	}

	err = mesh_create_buffers(mesh, model);
	if (err < 0) {
		return err;
	}

	return mesh_attach_textures(mesh, model);
}

int mesh_load(struct mesh *m, char *path, GLuint shader_program)
{
	int err;
	struct model model;

	err = loader_load_obj(&model, path);
	if (err < 0) {
		return err;
	}

	err = mesh_load_from_model(m, shader_program, &model);
	free_model(&model);

	return err;
}

void mesh_redraw(struct mesh *m)
{
	glBindVertexArray(m->vao);
	glBindTexture(GL_TEXTURE_2D, m->texture);
	glUseProgram(m->program);

	glDrawArrays(GL_TRIANGLES, 0, m->vertex_count);
}

void mesh_update_mvp(struct mesh *m, mat4x4 vp)
{
	mat4x4 mvp;

	mat4x4_mul(mvp, vp, m->model);

	glUseProgram(m->program);
	glUniformMatrix4fv(m->mvp_handle, 1, GL_FALSE, (float *)mvp);

	if (m->model_presented) {
		glUniformMatrix4fv(m->model_handle, 1, GL_FALSE, (float *)m->model);
	}

	if (m->time_presented) {
		glUniform1f(m->time_handle, global_time_counter);
	}
}

void mesh_free(struct mesh *m)
{
	glDeleteBuffers(1, &m->vbo);
	glDeleteBuffers(1, &m->nbo);
	glDeleteBuffers(1, &m->tbo);
}

