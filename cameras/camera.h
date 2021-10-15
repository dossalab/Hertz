#ifndef CAMERA_CAMERA_H
#define CAMERA_CAMERA_H

#include <utils/linmath.h>

struct camera {
	mat4x4 projection;
	mat4x4 vp;
};

#endif
