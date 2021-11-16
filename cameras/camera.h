#ifndef CAMERAS_CAMERA_H
#define CAMERAS_CAMERA_H

#include <utils/linmath.h>

struct camera {
	mat4x4 projection;
	mat4x4 vp;
};

#endif
