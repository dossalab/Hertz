#pragma once

#include <hz/node.h>
#include <stdbool.h>

bool assimp_import_scene(hz_node *root, hz_arena *arena, const char *path);
