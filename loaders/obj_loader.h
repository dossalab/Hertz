#ifndef LOADERS_OBJ_LOADER_H
#define LOADERS_OBJ_LOADER_H

#include "model.h"

struct stl_entry {
	struct vertex normal;
	struct vertex v1;
	struct vertex v2;
	struct vertex v3;
};

int loader_load_obj(struct model *model, const char *filename);

#endif

