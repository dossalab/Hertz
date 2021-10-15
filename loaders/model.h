#ifndef LOADERS_MODEL_H
#define LOADERS_MODEL_H

#include <utils/defines.h>
#include <stddef.h>

struct model {
	struct vertex *vertices;
	struct vertex *normals;
	struct point *uvs;

	size_t vertex_count;
	size_t normal_count;
	size_t uv_count;

	char *texture_path;
};

void free_model(struct model *m);

#endif
