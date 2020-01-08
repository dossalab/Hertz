#ifndef ENGINE_CAMERA_H
#define ENGINE_CAMERA_H

#include <engine/scene.h>
#include <engine/linmath.h>

struct camera {
	mat4x4 view;
};

void camera_init(struct camera *c);

#endif

