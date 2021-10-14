#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <logger/logger.h>
#include <gl/shaders.h>
#include <engine/scene.h>
#include <utils/linmath.h>
#include <errors/errors.h>

#define FLYTHROUGH_CAMERA_IMPLEMENTATION
#include "camera.h"

#define ENG_PI		3.14159265359f
#define ENG_FOV		(70.0f * ENG_PI / 180.0f)
#define WINDOW_TITLE	"My cool application"

static GLuint shader_sky;
static GLuint shader_horse;
static GLuint shader_land;
static mat4x4 projection;

static void update_projection(size_t width, size_t height)
{
	float aspect;

	aspect = (float)width / (float)height;

	mat4x4_perspective(projection, ENG_FOV, aspect, 0.1f, 1000.0f);
}

static void update_camera(GLFWwindow *window, struct scene *scene,
		mat4x4 view, float delta_time_sec)
{
	mat4x4 vp;
	double pos_x, pos_y;
	static double old_pos_x, old_pos_y;
	static const float up[3] = { 0.0f, 1.0f, 0.0f };
	static vec3 camera_position = { 5.0f, 5.0f, 5.0f };
	static vec3 camera_look = { 0.0f, 0.0f, 1.0f };

	glfwGetCursorPos(window, &pos_x, &pos_y);

	flythrough_camera_update(
		camera_position, camera_look, up,
		(float *)view,
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

	mat4x4_mul(vp, projection, view);
	scene_update_mvp(scene, vp);
}

static bool main_loop(GLFWwindow *window, struct scene *scene, mat4x4 view)
{
	double now; static double past;

	if (glfwWindowShouldClose(window)) {
		return false;
	}

	now = glfwGetTime();

	update_camera(window, scene, view, now - past);

	glfwPollEvents();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene_redraw(scene);
	glfwSwapBuffers(window);

	past = now;
	return true;
}

static int load_shaders(void)
{
	shader_sky = complile_simple_program("shaders/simple.vert",
			"shaders/skybox.frag");
	shader_horse = complile_simple_program("shaders/psycho.vert",
			"shaders/simple.frag");
	shader_land = complile_simple_program("shaders/simple.vert",
			"shaders/simple.frag");

	return shader_sky == 0 || shader_horse == 0 || shader_land == 0;
}

static int present_and_draw_scene(GLFWwindow *window)
{
	int err;

	struct mesh ship, skybox, land;
	struct scene scene;
	mat4x4 view;

	mat4x4_identity(view);

	err = mesh_load(&ship, "res/horse.obj", shader_horse);
	if (err < 0) {
		log_e("No horse!");
		return err;
	}

	err = mesh_load(&skybox, "res/skybox.obj", shader_sky);
	if (err < 0) {
		log_e("No skybox!");
		return err;
	}

	err = mesh_load(&land, "res/land.obj", shader_land);
	if (err < 0) {
		log_e("No land!");
		return err;
	}

	mat4x4_translate(land.model, 0.0f, -6.0f, 0.0f);

	scene_init(&scene);

	scene_add_mesh(&scene, &skybox);
	scene_add_mesh(&scene, &land);
	scene_add_mesh(&scene, &ship);
	// scene_add_mesh(&scene, &cube);

	for (;;) {
		if (!main_loop(window, &scene, view)) {
			break;
		}
	}

	scene_free(&scene);
	mesh_free(&ship);
	mesh_free(&land);
	mesh_free(&skybox);

	return 0;
}

static void window_resize_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	update_projection(width, height);
}

static GLFWwindow *create_context_window(void)
{
	GLenum glew_ret;
	GLFWwindow *window;

	window = glfwCreateWindow(640, 480, WINDOW_TITLE, NULL, NULL);
	if (!window) {
		log_e("unable to create window handle!");
		return NULL;
	}

	glfwMakeContextCurrent(window);

	/*
	 * uhh we have to do that *after* context is created.
	 * not sure what will happen if we create multiple windows
	 */
	glew_ret = glewInit();
	if (glew_ret != GLEW_OK) {
		log_e("unable to init GLEW! %s", glewGetErrorString(glew_ret));
		glfwDestroyWindow(window);
		return NULL;
	}

	glfwSetFramebufferSizeCallback(window, window_resize_callback);

	log_i("created window with GLEW %s", glewGetString(GLEW_VERSION));
	return window;
}

static int load_shaders_and_draw(GLFWwindow *window)
{
	int err;

	/* a couple of basic settings first */
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	err = load_shaders();
	if (err < 0) {
		log_e("unable to load shaders");
		return err;
	}

	err = present_and_draw_scene(window);
	/* TODO: remove shaders */

	return err;
}

static int create_window_and_draw(void)
{
	int err;
	GLFWwindow *window;

	window = create_context_window();
	if (!window) {
		log_e("unable to create a window!");
		return -ERR_NO_WINDOW;
	}

	err = load_shaders_and_draw(window);
	glfwDestroyWindow(window);

	return err;
}

static int gfx_main(void)
{
	int err;

	err = glfwInit();
	if (err < 0) {
		log_e("unable to init glfw!");
		return -ERR_SYSTEM;
	}

	err = create_window_and_draw();
	glfwTerminate();

	return err;
}

int main(void)
{
	int err;

	err = gfx_main();
	if (err < 0) {
		log_e("unable to run application - %s", error_to_string(err));
		return 1;
	}

	return 0;
}
