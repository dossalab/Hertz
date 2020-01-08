#ifndef ENGINE_ENGINE_H
#define ENGINE_ENGINE_H

#define GLFW_INCLUDE_ES3
#include <GLFW/glfw3.h>
#include <GLES3/gl3.h>

#include <engine/util.h>
#include <engine/scene.h>
#include <engine/camera.h>

#define ENG_PI		3.14159265359f
#define ENG_FOV		(70.0f * ENG_PI / 180.0f)

int engine_init(void);
int engine_poll(void);
void engine_set_scene(struct scene *s);
void engine_set_camera(struct camera *c);
void engine_update_mvp(void);
void engine_exit(void);

GLFWwindow *engine_get_window(void);

#endif

