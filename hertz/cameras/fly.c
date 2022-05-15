#include GL_EXTENSIONS_HEADER
#include <stdbool.h>
#include <hz/cameras/fly.h>
#include <vendor/linmath/linmath.h>
#include <hz/units.h>

static const vec3 up_direction = { 0.0f, 1.0f, 0.0f };

#define FLY_CAMERA_DEFAULT_SPEED	10.f
#define FLY_CAMERA_DEFAULT_SENSITIVITY	HZ_DEG_TO_RAD(0.5f)
#define FLY_CAMERA_DEFAULT_PITCH_MARGIN	HZ_DEG_TO_RAD(30.f)

static inline void mat4x4_mul_vec3(vec3 result, mat4x4 mat, vec3 vec) {
	vec4 ext_result;
	vec4 ext_vec = { vec[0], vec[1], vec[2], 1.0 };

	mat4x4_mul_vec4(ext_result, mat, ext_vec);
	vec3_dup(result, (vec4) { ext_result[0], ext_result[1], ext_result[2] });
}

static inline void mat4x4_rotate_vec(mat4x4 res, mat4x4 mat, vec3 vec, float a) {
	mat4x4_rotate(res, mat, vec[0], vec[1], vec[2], a);
}

static void apply_pitch_limits(struct hz_fly_camera *c, float *delta_pitch)
{
	struct hz_camera *super = HZ_CAMERA(c);
	float pitch = acosf(vec3_mul_inner(super->look, up_direction));

	if (pitch < c->pitch_margin && *delta_pitch > 0.0f) {
		*delta_pitch = 0.0f;
	}

	if (pitch > HZ_M_PI - c->pitch_margin && *delta_pitch < 0.0f) {
		*delta_pitch = 0.0f;
	}
}

void hz_fly_camera_move(struct hz_fly_camera *c, float dt, int dx, int dy,
		bool forward, bool left, bool backward, bool right)
{
	struct hz_camera *super = HZ_CAMERA(c);
	mat4x4 rotation;
	vec3 across, upward, x_vec, z_vec, xz_vec, look_point;
	float delta_yaw, delta_pitch, move_distance = c->speed * dt;

	mat4x4_identity(rotation);

	vec3_norm(super->look, super->look);
	vec3_mul_cross(across, super->look, up_direction);
	vec3_mul_cross(upward, across, super->look);

	if (right != left || forward != backward) {
		float x_multiplier = (right? 1.f : 0.f) - (left? 1.f : 0.f);
		float z_multiplier = (forward? 1.f : 0.f) - (backward? 1.f : 0.f);

		vec3_scale(x_vec, across, x_multiplier);
		vec3_scale(z_vec, super->look, z_multiplier);
		vec3_add(xz_vec, x_vec, z_vec);
		vec3_norm(xz_vec, xz_vec);
		vec3_scale(xz_vec, xz_vec, move_distance);
		vec3_add(super->position, super->position, xz_vec);
	}

	delta_yaw = -dx * c->sensitivity;
	delta_pitch = -dy * c->sensitivity;

	apply_pitch_limits(c, &delta_pitch);

	mat4x4_rotate_vec(rotation, rotation, upward, delta_yaw);
	mat4x4_rotate_vec(rotation, rotation, across, delta_pitch);

	mat4x4_mul_vec3(super->look, rotation, super->look);
	vec3_add(look_point, super->look, super->position);

	mat4x4_look_at(super->view, super->position, look_point, up_direction);
	mat4x4_mul(super->vp, super->projection, super->view);

	hz_camera_update_uniforms(super);
}

bool hz_fly_camera_init(struct hz_fly_camera *c, GLuint program)
{
	c->speed = FLY_CAMERA_DEFAULT_SPEED;
	c->sensitivity = FLY_CAMERA_DEFAULT_SENSITIVITY;
	c->pitch_margin = FLY_CAMERA_DEFAULT_PITCH_MARGIN;

	return hz_camera_init(HZ_CAMERA(c), program);
}
