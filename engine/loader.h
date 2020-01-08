#ifndef ENGINE_LOADER_H
#define ENGINE_LOADER_H

#include <engine/mesh.h>

struct stl_entry {
	struct vertex normal;
	struct vertex v1;
	struct vertex v2;
	struct vertex v3;
};

int loader_load(struct mesh *mesh, const char *filename);

#endif

