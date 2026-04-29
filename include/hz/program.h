#pragma once

#include <hz/util/arena.h>

typedef struct _hz_program hz_program;

hz_program *hz_program_new(hz_arena *arena, const char *vert, const char *frag);
