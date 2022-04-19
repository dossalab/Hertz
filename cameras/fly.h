#ifndef CAMERAS_FLY_H
#define CAMERAS_FLY_H

#include <utils/3rdparty/linmath/linmath.h>
#include <utils/common.h>
#include "camera.h"

struct hz_fly_camera {
	struct hz_camera as_camera;
	vec3 position, look, up;
	float speed;
};

#define hz_cast_fly_camera(ptr) \
	container_of(ptr, struct hz_fly_camera, as_camera)

extern const struct hz_camera_proto hz_fly_camera_proto;

#endif
