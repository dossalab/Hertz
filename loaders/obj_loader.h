#ifndef LOADERS_OBJ_LOADER_H
#define LOADERS_OBJ_LOADER_H

#include "model.h"
#include <utils/linmath.h>

struct stl_entry {
	vec3 normal;
	vec3 v1;
	vec3 v2;
	vec3 v3;
};

int loader_load_obj(struct model *model, const char *filename);

#endif

