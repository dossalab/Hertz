#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include <GLFW/glfw3.h>

#ifdef BUILD_WEBGL
#include <emscripten/emscripten.h>
#endif

#define FLYTHROUGH_CAMERA_IMPLEMENTATION
#include "camera.h"

#include <engine/api.h>

static const float up[3] = { 0.0f, 1.0f, 0.0f };

static struct camera main_camera;
static struct scene scene;
static struct mesh ship, skybox;

static vec3 camera_position = { 0.0f, 0.0f, 0.0f };
static vec3 camera_look = { 0.0f, 0.0f, 1.0f };

static void die(const char *fmt, ...)
{
	va_list params;

	va_start(params, fmt);
	vfprintf(stderr, fmt, params);
	va_end(params);

	#ifdef BUILD_WEBGL
		emscripten_force_exit(EXIT_FAILURE);
	#else
		exit(1);
	#endif
}

void update_camera(GLFWwindow *window, float delta_time_sec)
{
	double pos_x, pos_y;
	static double old_pos_x, old_pos_y;

	glfwGetCursorPos(window, &pos_x, &pos_y);

	flythrough_camera_update(
		camera_position, camera_look, up,
		(float *)main_camera.view,
		delta_time_sec,
		100.0f,
		0.5f,
		80.0f,
		pos_x - old_pos_x,
		pos_y - old_pos_y,
		glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS,
		glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS,
		glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS,
		glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS,
		false, false,
		0	
	);

	old_pos_x = pos_x;
	old_pos_y = pos_y;

	engine_update_mvp();
}

static void main_loop(void *user)
{
	double now; static double past;
	GLFWwindow *window;

	window = (GLFWwindow *)user;
	now = glfwGetTime();

	engine_poll();
	update_camera(window, now - past);

	past = now;
}

int main(int argc, char **argv)
{
	GLFWwindow *window;

	if (engine_init() < 0) {
		die("Unable to init engine!\n");
	}

	if (mesh_load(&ship, "res/ship.stl") < 0) {
		die("No ship!\n");
	}

	if (mesh_load(&skybox, "res/skybox.stl") < 0) {
		die("No skybox!\n");
	}

	camera_init(&main_camera);
	scene_init(&scene);

	scene_add_mesh(&scene, &skybox);
	scene_add_mesh(&scene, &ship);

	engine_set_scene(&scene);
	engine_set_camera(&main_camera);

	window = engine_get_window();

	#ifdef BUILD_WEBGL
		emscripten_set_main_loop_arg(main_loop, window, 0, 0);
	#else
		for (;;) {
			main_loop(window);
		}

		// should never appear here?
		scene_free(&scene);
		mesh_free(&ship);
		mesh_free(&skybox);
	#endif

	return 0;
}

