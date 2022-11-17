#pragma once

#include <hz/node.h>

typedef struct _hz_fly_actor hz_fly_actor;

void hz_fly_actor_move(hz_fly_actor *a, float dt, int dx, int dy,
		bool forward, bool left, bool backward, bool right);

hz_fly_actor *hz_fly_actor_new(hz_arena *arena, hz_node *puppet);
