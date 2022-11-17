#pragma once

#include <stdbool.h>

typedef struct _hz_arena hz_arena;

typedef struct {
	void (*free)(void *);
	unsigned size;
	const char *name;
} hz_arena_proto;

void *hz_arena_alloc(hz_arena *arena, const hz_arena_proto *proto);

hz_arena *hz_arena_new(void);
void hz_arena_free(hz_arena *arena);
