#ifndef CAMERAS_CAMERA_H
#define CAMERAS_CAMERA_H

#include <utils/3rdparty/linmath/linmath.h>

struct camera {
	mat4x4 projection, view, vp;
	vec3 position, look, up;
	float speed;
};

#endif
