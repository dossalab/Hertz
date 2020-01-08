#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <engine/util.h>
#include <engine/loader.h>

static void insert_vertex(struct mesh *m, struct vertex *v)
{
	m->vertices[m->vertice_cnt] = *v;
	m->vertice_cnt++;
}

static void insert_normal(struct mesh *m, struct vertex *v)
{
	m->normals[m->normal_cnt] = *v;
	m->normal_cnt++;
}

int loader_load(struct mesh *mesh, const char *filename)
{
	struct region data;
	void *ptr;
	int entries, size;

	if (read_path(filename,  &data)) {
		logger_report("[PS] Unable to load file %s\n", filename);
		return -1;
	}

	if (data.size <= 84) {
		logger_report("[PS] File %s is too small\n", filename);
		free(data.ptr);
		return -1;
	}

	entries = *((unsigned int *)(data.ptr + 80));
	size = 84 + entries * (sizeof(struct stl_entry) + 2);

	if (size > data.size) {
		logger_report("[PS] Seems like %s is corrupt!\n", filename);
		free(data.ptr);
		return -1;
	}

	ptr = data.ptr + 84; /* skip header + count*/
	logger_report("[PS] %d vertices\n", entries);

	mesh->vertices = malloc(sizeof(struct vertex) * size * 3);
	mesh->normals = malloc(sizeof(struct vertex) * size * 3);
	mesh->vertice_cnt = 0;
	mesh->normal_cnt = 0;
	mesh->tex_coord_cnt = 0;

	for (int i = 0; i < entries; i++) {
		struct stl_entry *entry = ptr;

		insert_vertex(mesh, &entry->v1);
		insert_vertex(mesh, &entry->v2);
		insert_vertex(mesh, &entry->v3);

		insert_normal(mesh, &entry->normal);
		insert_normal(mesh, &entry->normal);
		insert_normal(mesh, &entry->normal);

		ptr += sizeof(struct stl_entry);
		ptr += 2;
	}

	free(data.ptr);
	return 0;
}

