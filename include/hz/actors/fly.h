#ifndef HZ_ACTORS_FLY_H
#define HZ_ACTORS_FLY_H

#include <stdbool.h>

struct hz_fly_actor;
struct hz_object;

void hz_fly_actor_move(struct hz_fly_actor *a, float dt, int dx, int dy,
		bool forward, bool left, bool backward, bool right);

struct hz_fly_actor *hz_fly_actor_new(struct hz_object *puppet);

#endif
