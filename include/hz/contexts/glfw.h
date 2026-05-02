#pragma once

#include <stdbool.h>
#include <hz/context.h>
#include <hz/util/arena.h>

typedef struct _hz_glfw_context hz_glfw_context;

void hz_glfw_context_set_window_title(hz_glfw_context *context, const char *title);
void hz_glfw_context_set_cursor_hidden(hz_glfw_context *context, bool hidden);

hz_glfw_context *HZ_GLFW_CONTEXT(hz_context *super);
hz_context *hz_glfw_context_new(hz_arena *arena);
