#include <stdlib.h>
#include "model.h"

void free_model(struct model *m)
{
	free(m->vertices);
	free(m->normals);
	free(m->uvs);

	m->vertex_count = 0;
	m->normal_count = 0;
	m->uv_count = 0;
}
