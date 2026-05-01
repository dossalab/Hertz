#include <hz/adt/map.h>
#include <hz/util/mem.h>
#include <string.h>

#define INITIAL_CAPACITY	16

static unsigned hash_str(const char *s)
{
	/* FNV-1a */
	unsigned h = 2166136261u;
	while (*s) {
		h ^= (unsigned char)*s++;
		h *= 16777619u;
	}
	return h;
}

void hz_map_init(hz_map *map)
{
	map->capacity = INITIAL_CAPACITY;
	map->count    = 0;
	map->entries  = hz_malloc(INITIAL_CAPACITY * sizeof(hz_map_entry));
	memset(map->entries, 0, INITIAL_CAPACITY * sizeof(hz_map_entry));
}

void hz_map_cleanup(hz_map *map)
{
	for (size_t i = 0; i < map->capacity; i++)
		hz_free(map->entries[i].key);
	hz_free(map->entries);
}

void *hz_map_get(hz_map *map, const char *key)
{
	size_t h = hash_str(key) & (map->capacity - 1);

	for (size_t i = 0; i < map->capacity; i++) {
		size_t idx = (h + i) & (map->capacity - 1);

		if (!map->entries[idx].key)
			return NULL;
		if (strcmp(map->entries[idx].key, key) == 0)
			return map->entries[idx].value;
	}

	return NULL;
}

static void rehash_entry(hz_map_entry *entries, size_t capacity,
		char *key, void *value)
{
	size_t h = hash_str(key) & (capacity - 1);

	for (size_t i = 0; i < capacity; i++) {
		size_t idx = (h + i) & (capacity - 1);

		if (!entries[idx].key) {
			entries[idx].key   = key;
			entries[idx].value = value;
			return;
		}
	}
}

static void resize(hz_map *map)
{
	size_t new_cap     = map->capacity * 2;
	hz_map_entry *new_entries = hz_malloc(new_cap * sizeof(hz_map_entry));
	memset(new_entries, 0, new_cap * sizeof(hz_map_entry));

	for (size_t i = 0; i < map->capacity; i++) {
		if (map->entries[i].key)
			rehash_entry(new_entries, new_cap,
					map->entries[i].key, map->entries[i].value);
	}

	hz_free(map->entries);
	map->entries  = new_entries;
	map->capacity = new_cap;
}

void hz_map_set(hz_map *map, const char *key, void *value)
{
	size_t h = hash_str(key) & (map->capacity - 1);

	/* Update existing key */
	for (size_t i = 0; i < map->capacity; i++) {
		size_t idx = (h + i) & (map->capacity - 1);

		if (!map->entries[idx].key)
			break;
		if (strcmp(map->entries[idx].key, key) == 0) {
			map->entries[idx].value = value;
			return;
		}
	}

	/* Resize at 75% load before inserting */
	if (map->count * 4 >= map->capacity * 3) {
		resize(map);
		h = hash_str(key) & (map->capacity - 1);
	}

	for (size_t i = 0; i < map->capacity; i++) {
		size_t idx = (h + i) & (map->capacity - 1);

		if (!map->entries[idx].key) {
			map->entries[idx].key   = hz_malloc(strlen(key) + 1);
			strcpy(map->entries[idx].key, key);
			map->entries[idx].value = value;
			map->count++;
			return;
		}
	}
}
