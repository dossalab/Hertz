#include <stdlib.h>
#include "model.h"

void free_model(struct model *m)
{
	if (m->texture_path) {
		free(m->texture_path);
	}

	free(m->vertices);
	free(m->normals);
	free(m->uvs);

	m->vertex_count = 0;
	m->normal_count = 0;
	m->uv_count = 0;
}
