#pragma once

#include <hz/node.h>
#include <hz/util/arena.h>
#include <hz/material_store.h>

#define HZ_EXIT_CODE_OK		0
#define HZ_EXIT_CODE_ERROR	1

typedef struct _hz_context hz_context;

typedef hz_context *(*hz_context_constructor)(hz_arena *);
typedef bool (*hz_context_wrapper_init)(hz_context *, hz_arena *, hz_node *root, void *user);
typedef void (*hz_context_update_fn)(hz_context *context, float dt, void *user);

#define HZ_MOUSE_BUTTON_LEFT	0
#define HZ_MOUSE_BUTTON_RIGHT	1
#define HZ_MOUSE_BUTTON_MIDDLE	2

void hz_context_set_camera(hz_context *context, hz_node *camera);
bool hz_context_get_key(hz_context *context, char key);
bool hz_context_get_mouse_button(hz_context *context, int button);
void hz_context_get_cursor(hz_context *context, float *x, float *y);
float hz_context_get_scroll(hz_context *context);
hz_node *hz_context_get_camera(hz_context *context);
hz_material_store *hz_context_get_material_store(hz_context *context);
bool hz_context_poll(hz_context *context);
void hz_context_exit(hz_context *context);

int hz_context_wrapper_with_context(hz_arena *arena, hz_context *context,
		hz_context_wrapper_init init, hz_context_update_fn update, void *user);
int hz_context_wrapper_with_arena(hz_arena *arena, hz_context_constructor constructor,
		hz_context_wrapper_init init, hz_context_update_fn update, void *user);
int hz_context_wrapper(hz_context_constructor new_context,
		hz_context_wrapper_init init, hz_context_update_fn update, void *user);
