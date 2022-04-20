#ifndef HZ_CAMERAS_FLY_H
#define HZ_CAMERAS_FLY_H

#include <hz/utils/container_of.h>
#include <hz/camera.h>
#include <hz/types.h>

struct hz_fly_camera {
	struct hz_camera as_camera;
	hz_vec3 position, look, up;
	float speed;
};

#define hz_cast_fly_camera(ptr) \
	hz_container_of(ptr, struct hz_fly_camera, as_camera)

extern const struct hz_camera_proto hz_fly_camera_proto;

#endif
