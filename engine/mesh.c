#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

#include <GL/glew.h>
#include <logger/logger.h>
#include <errors/errors.h>
#include <gl/textures.h>
#include <loaders/obj_loader.h>
#include <loaders/stb_image.h>

#include "mesh.h"

static int mesh_prepare_gl(struct mesh *m, struct model *model)
{
	int vb_location, nb_location;

	glGenVertexArrays(1, &m->vao);
	glBindVertexArray(m->vao);

	vb_location = glGetAttribLocation(m->program, "in_position");
	nb_location = glGetAttribLocation(m->program, "in_normal");

	if (vb_location < 0) {
		log_e("unable to find normal or vertex buffer");
		return -ERR_SHADER_INVALID;
	}

	/* vertex buffer */
	glGenBuffers(1, &m->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
	glBufferData(GL_ARRAY_BUFFER, model->vertex_count * sizeof(struct vertex),
				model->vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(vb_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vb_location);

	if (nb_location >= 0) {
		/* normal buffer */
		glGenBuffers(1, &m->nbo);
		glBindBuffer(GL_ARRAY_BUFFER, m->nbo);
		glBufferData(GL_ARRAY_BUFFER, model->normal_count * sizeof(struct vertex),
						model->normals, GL_STATIC_DRAW);

		glVertexAttribPointer(nb_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(nb_location);
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

	log_i("loading texture from %s", model->texture_path);

	texture = create_texture_from_file(model->texture_path);
	if (!texture) {
		return -ERR_NO_FILE;
	}

	m->texture = texture;

	glGenBuffers(1, &m->tbo);
	glBindBuffer(GL_ARRAY_BUFFER, m->tbo);
	glBufferData(GL_ARRAY_BUFFER, model->uv_count * sizeof(struct point),
				model->uvs, GL_STATIC_DRAW);


	glVertexAttribPointer(tcb_location, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(tcb_location);

	return 0;
}

float get_time(void)
{
	static float tick;

	tick += 0.05f;

	return tick;
}

void mesh_update_mvp(struct mesh *m, mat4x4 pv)
{
	mat4x4 mvp;
	int mvp_pos;
	int model_pos;
	int time_pos;

	mat4x4_mul(mvp, pv, m->model);

	mvp_pos = glGetUniformLocation(m->program, "MVP");
	model_pos = glGetUniformLocation(m->program, "model");
	time_pos = glGetUniformLocation(m->program, "time");

	glUseProgram(m->program);
	glUniformMatrix4fv(mvp_pos, 1, GL_FALSE, (float *)mvp);
	glUniformMatrix4fv(model_pos, 1, GL_FALSE, (float *)m->model);
	glUniform1f(time_pos, get_time());
}

void mesh_redraw(struct mesh *m)
{
	glBindVertexArray(m->vao);
	glBindTexture(GL_TEXTURE_2D, m->texture);
	glUseProgram(m->program);

	glDrawArrays(GL_TRIANGLES, 0, m->vertex_count);
}

static int mesh_load_from_model(struct mesh *mesh, GLuint shader_program,
		struct model *model)
{
	int err;

	mat4x4_identity(mesh->model);
	mesh->program = shader_program;
	mesh->vertex_count = model->vertex_count;

	err = mesh_prepare_gl(mesh, model);
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

void mesh_free(struct mesh *m)
{
	glDeleteBuffers(1, &m->vbo);
	glDeleteBuffers(1, &m->nbo);
	glDeleteBuffers(1, &m->tbo);
}

