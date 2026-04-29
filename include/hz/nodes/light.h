#pragma once

#include <hz/node.h>
#include <hz/program.h>

typedef struct _hz_light hz_light;

hz_light *HZ_LIGHT(hz_node *o);

void hz_light_setup(hz_light *l, float kc, float kl, float kq);
void hz_light_dim(hz_light *l, float intensity);

hz_node *hz_light_new(hz_arena *arena, hz_program *program, unsigned index);
