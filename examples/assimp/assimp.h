#pragma once

#include <hz/node.h>
#include <hz/program.h>
#include <hz/material_store.h>
#include <stdbool.h>

bool assimp_import_scene(hz_node *root, hz_arena *arena, hz_material_store *store,
		const char *path, hz_program *program);
