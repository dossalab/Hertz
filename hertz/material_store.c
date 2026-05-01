#include <hz/material_store.h>
#include <hz/adt/map.h>

struct _hz_material_store {
	hz_map map;
};

static void store_free(void *ptr)
{
	hz_material_store *store = ptr;
	hz_map_cleanup(&store->map);
}

static const hz_arena_proto store_arena_proto = {
	.name = "material_stores",
	.free = store_free,
	.size = sizeof(hz_material_store),
};

hz_material_store *hz_material_store_new(hz_arena *arena)
{
	hz_material_store *store = hz_arena_alloc(arena, &store_arena_proto);
	hz_map_init(&store->map);
	return store;
}

hz_material *hz_material_store_get(hz_material_store *store, const char *name)
{
	return hz_map_get(&store->map, name);
}

void hz_material_store_set(hz_material_store *store, const char *name, hz_material *material)
{
	hz_map_set(&store->map, name, material);
}

hz_material *hz_material_store_default(hz_material_store *store, hz_arena *arena)
{
	hz_material *m = hz_material_store_get(store, "default");
	if (!m) {
		m = hz_material_new(arena);
		hz_material_store_set(store, "default", m);
	}
	return m;
}
