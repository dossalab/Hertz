#pragma once

#include <stddef.h>

typedef struct {
	char  *key;
	void  *value;
} hz_map_entry;

typedef struct {
	hz_map_entry *entries;
	size_t        capacity;
	size_t        count;
} hz_map;

void  hz_map_init(hz_map *map);
void  hz_map_cleanup(hz_map *map);
void  hz_map_set(hz_map *map, const char *key, void *value);
void *hz_map_get(hz_map *map, const char *key);
