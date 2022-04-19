#ifndef CAMERAS_CAMERA_H
#define CAMERAS_CAMERA_H

#include <utils/3rdparty/linmath/linmath.h>
#include <utils/glfw_context.h>

struct camera;

struct camera_proto {
	void (*init)(struct camera *);
	void (*update)(struct camera *, GLFWwindow *, float);
};

struct camera {
	mat4x4 view, projection, vp;
	const struct camera_proto *proto;
};

void camera_update_perspective(struct camera *c, float aspect);
void camera_update(struct camera *c, GLFWwindow *window, float time_spent);
void camera_init(struct camera *c, const struct camera_proto *proto);

#endif
