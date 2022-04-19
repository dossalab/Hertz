#include "camera.h"

#define ENG_PI		3.14159265359f
#define ENG_FOV		(70.0f * ENG_PI / 180.0f)

void hz_camera_update_perspective(struct hz_camera *c, float aspect)
{
	mat4x4_perspective(c->projection, ENG_FOV, aspect, 0.1f, 1000.0f);
	mat4x4_mul(c->vp, c->projection, c->view);
}

void hz_camera_update(struct hz_camera *c, GLFWwindow *window, float time_spent)
{
	c->proto->update(c, window, time_spent);
	mat4x4_mul(c->vp, c->projection, c->view);
}

void hz_camera_init(struct hz_camera *c, const struct hz_camera_proto *proto)
{
	c->proto = proto;

	mat4x4_identity(c->projection);
	mat4x4_identity(c->view);
	mat4x4_identity(c->vp);

	c->proto->init(c);
}
