#include <hz/actors/orbit.h>
#include <hz/util/arena.h>
#include <hz/units.h>
#include <hz/node.h>
#include <vendor/linmath/linmath.h>

#define ORBIT_DEFAULT_RADIUS		5.f
#define ORBIT_DEFAULT_SENSITIVITY	HZ_DEG_TO_RAD(0.3f)

struct _hz_orbit_actor {
	mat4x4 rotation;
	float radius, sensitivity;
	hz_node *puppet;
};

void hz_orbit_actor_rotate(hz_orbit_actor *a, float dx, float dy)
{
	mat4x4 model, translate;
	vec2 prev = { 0.f, 0.f };
	vec2 curr = {
		-dx * a->sensitivity,
		 dy * a->sensitivity,
	};

	mat4x4_arcball(a->rotation, a->rotation, prev, curr, 1.f);

	mat4x4_translate(translate, 0.f, 0.f, a->radius);
	mat4x4_mul(model, a->rotation, translate);

	hz_node_set_model(a->puppet, model, false);
}

static void orbit_actor_init(hz_orbit_actor *a, hz_node *puppet)
{
	a->radius      = ORBIT_DEFAULT_RADIUS;
	a->sensitivity = ORBIT_DEFAULT_SENSITIVITY;
	a->puppet      = puppet;
	mat4x4_identity(a->rotation);
}

static const hz_arena_proto hz_orbit_actor_arena_proto = {
	.name = "orbitactors",
	.size = sizeof(hz_orbit_actor),
};

hz_orbit_actor *hz_orbit_actor_new(hz_arena *arena, hz_node *puppet)
{
	hz_orbit_actor *actor = hz_arena_alloc(arena, &hz_orbit_actor_arena_proto);
	orbit_actor_init(actor, puppet);

	return actor;
}
