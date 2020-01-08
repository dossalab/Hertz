#include <stdlib.h>
#include <stdint.h>

#include <engine/engine.h>
#include <engine/mesh.h>
#include <engine/loader.h>
#include <engine/shader.h>
#include <engine/stb_image.h>

static int mesh_attach_geometry(struct mesh *m)
{
	int vb_location, nb_location;

	vb_location = glGetAttribLocation(m->program, "in_position");
	nb_location = glGetAttribLocation(m->program, "in_normal");

	if (vb_location < 0 || nb_location < 0) {
		return -1;
	}

	/* vertex buffer */
	glGenBuffers(1, &m->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
	glBufferData(GL_ARRAY_BUFFER, m->vertice_cnt * sizeof(struct vertex),
				m->vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(vb_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vb_location);

	/* normal buffer */
	glGenBuffers(1, &m->nbo);
	glBindBuffer(GL_ARRAY_BUFFER, m->nbo);
	glBufferData(GL_ARRAY_BUFFER, m->normal_cnt * sizeof(struct vertex),
				m->normals, GL_STATIC_DRAW);

	glVertexAttribPointer(nb_location, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(nb_location);

	return 0;
}

static int mesh_attach_textures(struct mesh *m)
{
	int w, h, n;
	uint8_t *data;
	int tcb_location;

	tcb_location = glGetAttribLocation(m->program, "in_texcoords");
	if (tcb_location < 0) {
		return -1;
	}

	data = stbi_load("res/test.jpg", &w, &h, &n, 0);
	if (!data) {
		return -1;
	}

	glGenTextures(1, &m->texture);
	glBindTexture(GL_TEXTURE_2D, m->texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
			w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glGenerateMipmap(GL_TEXTURE_2D);

	glGenBuffers(1, &m->tbo);
	glBindBuffer(GL_ARRAY_BUFFER, m->tbo);
	glBufferData(GL_ARRAY_BUFFER, m->tex_coord_cnt * sizeof(struct point),
				m->tex_coords, GL_STATIC_DRAW);

	glVertexAttribPointer(tcb_location, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(tcb_location);

	stbi_image_free(data);
	return 0;
}

static int mesh_attach(struct mesh *m)
{
	glGenVertexArrays(1, &m->vao);
	glBindVertexArray(m->vao);

	if (mesh_attach_geometry(m) < 0) {
		return -1;
	}

	if (mesh_attach_textures(m) < 0) {
		return -1;
	}

	return 0;
}

void mesh_update_mvp(struct mesh *m, mat4x4 pv)
{
	mat4x4 mvp;
	int mvp_pos;
	int model_pos;

	mat4x4_mul(mvp, pv, m->model);

	mvp_pos = glGetUniformLocation(m->program, "MVP");
	model_pos = glGetUniformLocation(m->program, "model");

	glUseProgram(m->program);
	glUniformMatrix4fv(mvp_pos, 1, GL_FALSE, (float *)mvp);
	glUniformMatrix4fv(model_pos, 1, GL_FALSE, (float *)m->model);
}

void mesh_redraw(struct mesh *m)
{
	glBindVertexArray(m->vao);
	glUseProgram(m->program);

	glDrawArrays(GL_TRIANGLES, 0, m->vertice_cnt);
}

int mesh_load(struct mesh *m, char *path)
{
	if (loader_load(m, path)) {
		return -1;
	}

	m->tex_coords = malloc(sizeof(struct point) * m->vertice_cnt);
	m->tex_coord_cnt = m->vertice_cnt;
	for (int i = 0; i < m->vertice_cnt; i++) {
		switch (i % 3) {
		case 0:
			m->tex_coords[i].x = 0.0;
			m->tex_coords[i].y = 0.0;
			break;
		case 1:
			m->tex_coords[i].x = 1.0;
			m->tex_coords[i].y = 1.0;
			break;
		case 2:
			m->tex_coords[i].x = 0.0;
			m->tex_coords[i].y = 1.0;
			break;
		}

	}

	if (shaders_apply(m, 2, "shaders/simple.vert",
				"shaders/simple.frag" ) < 0)
	{
		return -1;
	}

	if (mesh_attach(m) < 0) {
		return -1;
	}

	mat4x4_identity(m->model);
	return 0;
}

void mesh_free(struct mesh *m)
{
	free(m->vertices);
	free(m->normals);
	free(m->tex_coords);

	m->vertice_cnt = 0;
	m->normal_cnt = 0;
	m->tex_coord_cnt = 0;

	glDeleteBuffers(1, &m->vbo);
	glDeleteBuffers(1, &m->nbo);
	glDeleteBuffers(1, &m->tbo);
}

