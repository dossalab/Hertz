#ifndef RENDER_RENDER_H
#define RENDER_RENDER_H

#include <stdbool.h>
#include <cameras/fly_camera.h>
#include "scene.h"

struct render_state {
	struct scene scene;
	struct camera fly_camera;
	double time;
	GLuint shader_sky;
	GLuint shader_horse;
	GLuint shader_land;

	struct glfw_ctx_callbacks glfw_callbacks;
};

bool render_init(struct render_state *state);
void render_exit(struct render_state *state);

#endif
