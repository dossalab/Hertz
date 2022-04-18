#include <math.h>
#include <assert.h>
#include <stdbool.h>
#include <utils/3rdparty/linmath/linmath.h>

#include "fly_camera.h"

#define CAM_PI	3.14159265359f

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

static void apply_pitch_limits(struct camera *c, float *delta_pitch)
{
	float pitch = acosf(vec3_mul_inner(c->look, c->up));

	if (pitch < pitch_margin && *delta_pitch > 0.0f) {
		*delta_pitch = 0.0f;
	}

	if (pitch > CAM_PI - pitch_margin && *delta_pitch < 0.0f) {
		*delta_pitch = 0.0f;
	}
}

void fly_camera_update_helper(struct camera *c, float time_spent, int dx, int dy,
	bool forward, bool left, bool backward, bool right)
{
	mat4x4 rotation;
	vec3 across, upward, x_vec, z_vec, xz_vec, look_point;
	float delta_yaw, delta_pitch, move_distance = c->speed * time_spent;

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
		vec3_add(c->position, c->position, xz_vec);
	}

	delta_yaw = -dx * sensitivity;
	delta_pitch = -dy * sensitivity;

	apply_pitch_limits(c, &delta_pitch);

	mat4x4_rotate_vec(rotation, rotation, upward, delta_yaw);
	mat4x4_rotate_vec(rotation, rotation, across, delta_pitch);

	mat4x4_mul_vec3(c->look, rotation, c->look);
	vec3_add(look_point, c->look, c->position);
	mat4x4_look_at(c->view, c->position, look_point, c->up);
}

void fly_camera_update(struct camera *c, GLFWwindow *window, float time_spent)
{
	double pos_x, pos_y;
	static double old_pos_x, old_pos_y;

	glfwGetCursorPos(window, &pos_x, &pos_y);

	fly_camera_update_helper(c,
		time_spent,
		pos_x - old_pos_x,
		pos_y - old_pos_y,
		glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS,
		glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS,
		glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS,
		glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);

	mat4x4_mul(c->vp, c->projection, c->view);

	old_pos_x = pos_x;
	old_pos_y = pos_y;
}

void fly_camera_update_projection(struct camera *c, float fov, float aspect)
{
	mat4x4_perspective(c->projection, fov, aspect, 0.1f, 1000.0f);
}

void fly_camera_reset(struct camera *c)
{
	c->speed = 10.f;

	mat4x4_identity(c->projection);
	mat4x4_identity(c->view);
	mat4x4_identity(c->vp);

	vec3_dup(c->look, (vec3) { 1.0f, 1.0f, 1.0f });
	vec3_dup(c->position, (vec3) { 5.0f, 5.0f, 5.0f });
	vec3_dup(c->up, (vec3) { 0.0f, 1.0f, 0.0f });
}
