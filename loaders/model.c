#include <stdlib.h>
#include "model.h"

void free_model(struct model *m)
{
	free(m->vertices);
	free(m->normals);
	free(m->uvs);
}
