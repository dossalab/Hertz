#include HZ_GL_EXTENSIONS_HEADER
#include <stdbool.h>
#include <hz/actors/fly.h>
#include <vendor/linmath/linmath.h>
#include <hz/units.h>
#include <internal/alloc.h>
#include <hz/object.h>

#define FLY_ACTOR_DEFAULT_SPEED		10.f
#define FLY_ACTOR_DEFAULT_SENSITIVITY	HZ_DEG_TO_RAD(0.5f)
#define FLY_ACTOR_DEFAULT_PITCH_MARGIN	HZ_DEG_TO_RAD(30.f)
#define FLY_ACTOR_DEFAULT_LOOK		(vec3) { 1.0f, 1.0f, 1.0f }
#define FLY_ACTOR_DEFAULT_POSITION	(vec3) { 0.0f, 0.0f, 0.0f }

struct hz_fly_actor {
	vec3 look, position;
	float speed, sensitivity, pitch_margin;
	struct hz_object *puppet;
	float yaw, pitch;
};

static void apply_pitch_limits(struct hz_fly_actor *a)
{
	// if (a->pitch < HZ_M_PI / 2.f + a->pitch_margin) {
	// 	a->pitch = HZ_M_PI / 2.f + a->pitch_margin;
	// }

	if (a->pitch > HZ_M_PI - a->pitch_margin) {
		a->pitch = HZ_M_PI - a->pitch_margin;
	}
}

void hz_fly_actor_move(struct hz_fly_actor *a, float dt, int dx, int dy,
		bool forward, bool left, bool backward, bool right)
{
	vec3 look = {0.f, 0.f, -1.f};
	vec3 upward = {0.f, 1.f, 0.f};
	vec3 across = {1.f, 0.f, 0.f};
	vec3 x_vec, z_vec, xz_vec;
	float move_distance = a->speed * dt;

	a->yaw += -dx * a->sensitivity;
	a->pitch += -dy * a->sensitivity;

	apply_pitch_limits(a);

	quat qy, qp, q;

	quat_rotate(qy, a->yaw, (vec3) { 0.f, 1.f, 0.f});
	quat_rotate(qp, a->pitch, (vec3) { 1.f, 0.f, 0.f});
	quat_mul(q, qy, qp);

	quat_mul_vec3(look, q, look);
	quat_mul_vec3(upward, q, upward);
	quat_mul_vec3(across, q, across);

	if (right != left || forward != backward) {
		float x_multiplier = (right? 1.f : 0.f) - (left? 1.f : 0.f);
		float z_multiplier = (forward? 1.f : 0.f) - (backward? 1.f : 0.f);

		vec3_scale(x_vec, across, x_multiplier);
		vec3_scale(z_vec, look, z_multiplier);
		vec3_add(xz_vec, x_vec, z_vec);
		vec3_norm(xz_vec, xz_vec);
		vec3_scale(xz_vec, xz_vec, move_distance);

		vec3_add(a->position, a->position, xz_vec);
		hz_object_move(a->puppet, a->position);
	}

	hz_object_rotate_quat(a->puppet, q);
}

bool fly_actor_init(struct hz_fly_actor *a, struct hz_object *puppet)
{
	a->speed = FLY_ACTOR_DEFAULT_SPEED;
	a->sensitivity = FLY_ACTOR_DEFAULT_SENSITIVITY;
	a->pitch_margin = FLY_ACTOR_DEFAULT_PITCH_MARGIN;
	a->puppet = puppet;

	vec3_dup(a->look, FLY_ACTOR_DEFAULT_LOOK);
	vec3_dup(a->position, FLY_ACTOR_DEFAULT_POSITION);
	a->yaw = 0.f;
	a->pitch = 0.f;

	return true;
}

struct hz_fly_actor *hz_fly_actor_new(struct hz_object *puppet)
{
	return hz_alloc_and_init(struct hz_fly_actor, fly_actor_init, puppet);
}
