#ifndef LOADERS_MODEL_H
#define LOADERS_MODEL_H

#include <stddef.h>
#include <utils/linmath.h>

struct model {
	vec3 *vertices;
	vec3 *normals;
	vec2 *uvs;

	size_t vertex_count;
	size_t normal_count;
	size_t uv_count;

	char *texture_path;
};

void free_model(struct model *m);

#endif
