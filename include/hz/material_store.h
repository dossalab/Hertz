#pragma once

#include <hz/material.h>
#include <hz/util/arena.h>

typedef struct _hz_material_store hz_material_store;

hz_material_store *hz_material_store_new(hz_arena *arena);
void               hz_material_store_set(hz_material_store *store, const char *name, hz_material *material);
hz_material       *hz_material_store_get(hz_material_store *store, const char *name);
hz_material       *hz_material_store_default(hz_material_store *store, hz_arena *arena);
