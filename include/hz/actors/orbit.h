#pragma once

#include <hz/node.h>

typedef struct _hz_orbit_actor hz_orbit_actor;

void hz_orbit_actor_rotate(hz_orbit_actor *a, float dx, float dy);

hz_orbit_actor *hz_orbit_actor_new(hz_arena *arena, hz_node *puppet);
