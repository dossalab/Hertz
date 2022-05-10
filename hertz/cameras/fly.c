#include GL_EXTENSIONS_HEADER
#include <stdbool.h>
#include <hz/cameras/fly.h>
#include <vendor/linmath/linmath.h>

#define CAM_PI	3.14159265359f
#define CAM_FOV	(70.0f * CAM_PI / 180.0f)

static const float pitch_margin = 0.5f;
static const float sensitivity = 0.5f * CAM_PI / 180.0f;

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
	float pitch = acosf(vec3_mul_inner(c->look, c->up));

	if (pitch < pitch_margin && *delta_pitch > 0.0f) {
		*delta_pitch = 0.0f;
	}

	if (pitch > CAM_PI - pitch_margin && *delta_pitch < 0.0f) {
		*delta_pitch = 0.0f;
	}
}

void hz_fly_camera_move(struct hz_fly_camera *c, float dt, int dx, int dy,
		bool forward, bool left, bool backward, bool right)
{
	struct hz_camera *super = hz_cast_camera(c);
	mat4x4 rotation;
	vec3 across, upward, x_vec, z_vec, xz_vec, look_point;
	float delta_yaw, delta_pitch, move_distance = c->speed * dt;

	mat4x4_identity(rotation);

	vec3_norm(c->look, c->look);
	vec3_mul_cross(across, c->look, c->up);
	vec3_mul_cross(upward, across, c->look);

	if (right != left || forward != backward) {
		float x_multiplier = (right? 1.f : 0.f) - (left? 1.f : 0.f);
		float z_multiplier = (forward? 1.f : 0.f) - (backward? 1.f : 0.f);

		vec3_scale(x_vec, across, x_multiplier);
		vec3_scale(z_vec, c->look, z_multiplier);
		vec3_add(xz_vec, x_vec, z_vec);
		vec3_norm(xz_vec, xz_vec);
		vec3_scale(xz_vec, xz_vec, move_distance);
		vec3_add(super->position, super->position, xz_vec);
	}

	delta_yaw = -dx * sensitivity;
	delta_pitch = -dy * sensitivity;

	apply_pitch_limits(c, &delta_pitch);

	mat4x4_rotate_vec(rotation, rotation, upward, delta_yaw);
	mat4x4_rotate_vec(rotation, rotation, across, delta_pitch);

	mat4x4_mul_vec3(c->look, rotation, c->look);
	vec3_add(look_point, c->look, super->position);

	mat4x4_look_at(super->view, super->position, look_point, c->up);
	mat4x4_mul(super->vp, super->projection, super->view);

	glUniform3fv(super->uniforms.position, 1, super->position);
}

static void fly_camera_update(struct hz_camera *super, size_t w, size_t h)
{
	float aspect;

	aspect = (float)w / h;

	mat4x4_perspective(super->projection, CAM_FOV, aspect, 0.1f, 1000.0f);
	mat4x4_mul(super->vp, super->projection, super->view);
}

static void fly_camera_probe(struct hz_camera *super)
{
	struct hz_fly_camera *c = hz_cast_fly_camera(super);

	c->speed = 10.f;

	vec3_dup(c->look, (vec3) { 1.0f, 1.0f, 1.0f });
	vec3_dup(super->position, (vec3) { 5.0f, 5.0f, 5.0f });
	vec3_dup(c->up, (vec3) { 0.0f, 1.0f, 0.0f });
}

const struct hz_camera_proto hz_fly_camera_proto = {
	.probe = fly_camera_probe,
	.update = fly_camera_update
};

bool hz_fly_camera_init(struct hz_fly_camera *c, GLuint program)
{
	struct hz_camera *super = hz_cast_camera(c);
	return hz_camera_init(super, &hz_fly_camera_proto, program);
}
