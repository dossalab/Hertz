#pragma once

#include <hz/node.h>
#include <hz/util/arena.h>

#define HZ_EXIT_CODE_OK		0
#define HZ_EXIT_CODE_ERROR	1

typedef struct _hz_context hz_context;

typedef hz_context *(*hz_context_constructor)(hz_arena *);
typedef bool (*hz_context_wrapper_init)(hz_context *, hz_arena *, hz_node *root, int *shader, void *user);

bool hz_context_get_key(hz_context *context, char key);
bool hz_context_poll(hz_context *context);
void hz_context_exit(hz_context *context);

int hz_context_wrapper_with_context(hz_arena *arena, hz_context *context,
		hz_context_wrapper_init init, void *user);
int hz_context_wrapper_with_arena(hz_arena *arena, hz_context_constructor constructor,
		hz_context_wrapper_init init, void *user);
int hz_context_wrapper(hz_context_constructor new_context, hz_context_wrapper_init init, void *user);