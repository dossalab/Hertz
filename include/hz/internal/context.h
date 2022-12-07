#pragma once

#include <stdbool.h>
#include <hz/context.h>
#include <hz/util/arena.h>
#include <hz/nodes/camera.h>

typedef struct {
	void (*exit)(hz_context *super);
	bool (*poll)(hz_context *super);
	bool (*get_key)(hz_context *super, char key);
	hz_arena_proto arena_proto;
} hz_context_proto;

struct _hz_context {
	const hz_context_proto *proto;
	hz_node *camera;
};

void hz_context_handle_resize(hz_context *context, unsigned w, unsigned h);
void hz_context_init(hz_context *context, const hz_context_proto *proto);

#define HZ_CONTEXT(context)	&(context)->super

#define hz_context_new(arena, type, proto) ({ \
	type *context = hz_arena_alloc(arena, &(proto)->arena_proto); \
	if (!context) { \
		return NULL; \
	} \
	hz_context_init(HZ_CONTEXT(context), proto); \
	HZ_CONTEXT(context); \
})
