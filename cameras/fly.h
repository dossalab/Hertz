#ifndef CAMERAS_FLY_H
#define CAMERAS_FLY_H

#include <render/camera.h>
#include <utils/3rdparty/linmath/linmath.h>
#include <utils/common.h>

struct fly_camera {
	struct camera as_camera;
	vec3 position, look, up;
	float speed;
};

#define cast_fly_camera(ptr) \
	container_of(ptr, struct fly_camera, as_camera)

extern const struct camera_proto fly_camera_proto;

#endif
