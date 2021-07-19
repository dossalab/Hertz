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
static struct mesh ship, skybox, cube, land;

static vec3 camera_position = { 5.0f, 5.0f, 5.0f };
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
		10.0f,
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

static int main_loop(void *user)
{
	double now; static double past;
	GLFWwindow *window;

	window = (GLFWwindow *)user;
	now = glfwGetTime();

	if (engine_poll() < 0) {
		return -1;
	}

	update_camera(window, now - past);

	past = now;
	return 0;
}

#define SHADERS_SKY	"shaders/simple.vert", "shaders/skybox.frag"
#define SHADERS_SIMPLE	"shaders/simple.vert", "shaders/simple.frag"
#define SHADERS_HORSE	"shaders/psycho.vert", "shaders/simple.frag"
#define SHADERS_LAND	"shaders/simple.vert", "shaders/simple.frag"

int main(int argc, char **argv)
{
	GLFWwindow *window;

	if (engine_init() < 0) {
		die("Unable to init engine!\n");
	}

	if (mesh_load(&ship, "res/horse.obj", SHADERS_HORSE) < 0) {
		die("No horse!\n");
	}

	if (mesh_load(&skybox, "res/skybox.obj", SHADERS_SKY) < 0) {
		die("No skybox!\n");
	}

	// if (mesh_load(&cube, "res/cube.obj", SHADERS_SIMPLE) < 0) {
	// 	die("No cube!\n");
	// }

	if (mesh_load(&land, "res/land.obj", SHADERS_LAND) < 0) {
		die("No land!\n");
	}

	mat4x4_translate(land.model, 0.0f, -6.0f, 0.0f);

	camera_init(&main_camera);
	scene_init(&scene);

	scene_add_mesh(&scene, &skybox);
	scene_add_mesh(&scene, &land);
	scene_add_mesh(&scene, &ship);
	// scene_add_mesh(&scene, &cube);

	engine_set_scene(&scene);
	engine_set_camera(&main_camera);

	window = engine_get_window();

	#ifdef BUILD_WEBGL
		emscripten_set_main_loop_arg(main_loop, window, 0, 0);
	#else
		for (;;) {
			if (main_loop(window) < 0) {
				break;
			}
		}

		scene_free(&scene);
		mesh_free(&ship);
		mesh_free(&land);
		mesh_free(&skybox);
	#endif

	printf("end.");

	return 0;
}

