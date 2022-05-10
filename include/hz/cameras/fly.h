#ifndef HZ_CAMERAS_FLY_H
#define HZ_CAMERAS_FLY_H

#include <hz/utils/container_of.h>
#include <hz/camera.h>
#include <hz/types.h>

struct hz_fly_camera {
	struct hz_camera super;
	hz_vec3 look, up;
	float speed;
};

#define hz_cast_fly_camera(ptr) \
	hz_container_of(ptr, struct hz_fly_camera, super)

void hz_fly_camera_move(struct hz_fly_camera *c, float dt, int dx, int dy,
		bool forward, bool left, bool backward, bool right);
bool hz_fly_camera_init(struct hz_fly_camera *c, GLuint program);

#endif
