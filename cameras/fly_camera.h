#ifndef CAMERAS_FLY_CAMERA_H
#define CAMERAS_FLY_CAMERA_H

#include <GLFW/glfw3.h>
#include "camera.h"

void fly_camera_update_projection(struct camera *c, float fov, float aspect);
void fly_camera_update(struct camera *c, GLFWwindow *window, float time_spent);

void fly_camera_reset(struct camera *c);

#endif
