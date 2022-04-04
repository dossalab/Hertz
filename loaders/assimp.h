#ifndef LOADERS_ASSIMP_H
#define LOADERS_ASSIMP_H

#include <stdbool.h>

struct scene;

bool assimp_import_scene(const char *path, struct scene *s);

#endif
