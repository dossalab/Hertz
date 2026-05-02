#pragma once

#include <hz/node.h>

typedef struct _hz_fly_actor hz_fly_actor;

void hz_fly_actor_move(hz_fly_actor *a, float dt, float dx, float dy,
		bool forward, bool left, bool backward, bool right);
void hz_fly_actor_set_position(hz_fly_actor *a, hz_vec3 pos);

hz_fly_actor *hz_fly_actor_new(hz_arena *arena, hz_node *puppet);
