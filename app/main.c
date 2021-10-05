#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <logger/logger.h>
#include <gl/utils/shaders.h>
#include <engine/engine.h>
#include <engine/scene.h>

#define WINDOW_TITLE		"My cool application"
#define FLYTHROUGH_CAMERA_IMPLEMENTATION
#include "camera.h"

static GLFWwindow *main_window;
static const float up[3] = { 0.0f, 1.0f, 0.0f };

static struct camera main_camera;
static struct scene scene;
static struct mesh ship, skybox, cube, land;

static vec3 camera_position = { 5.0f, 5.0f, 5.0f };
static vec3 camera_look = { 0.0f, 0.0f, 1.0f };

#define SHADERS_SKY	"shaders/simple.vert", "shaders/skybox.frag"
#define SHADERS_SIMPLE	"shaders/simple.vert", "shaders/simple.frag"
#define SHADERS_HORSE	"shaders/psycho.vert", "shaders/simple.frag"
#define SHADERS_LAND	"shaders/simple.vert", "shaders/simple.frag"

static GLuint shader_sky;
static GLuint shader_simple;
static GLuint shader_horse;
static GLuint shader_land;

void update_camera(float delta_time_sec)
{
	double pos_x, pos_y;
	static double old_pos_x, old_pos_y;

	glfwGetCursorPos(main_window, &pos_x, &pos_y);

	flythrough_camera_update(
		camera_position, camera_look, up,
		(float *)main_camera.view,
		delta_time_sec,
		10.0f,
		0.5f,
		80.0f,
		pos_x - old_pos_x,
		pos_y - old_pos_y,
		glfwGetKey(main_window, GLFW_KEY_W) == GLFW_PRESS,
		glfwGetKey(main_window, GLFW_KEY_A) == GLFW_PRESS,
		glfwGetKey(main_window, GLFW_KEY_S) == GLFW_PRESS,
		glfwGetKey(main_window, GLFW_KEY_D) == GLFW_PRESS,
		false, false,
		0	
	);

	old_pos_x = pos_x;
	old_pos_y = pos_y;

	engine_update_mvp();
}

static int main_loop(void)
{
	double now; static double past;

	if (glfwWindowShouldClose(main_window)) {
		return -1;
	}

	now = glfwGetTime();

	update_camera(now - past);

	glfwPollEvents();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	engine_redraw();
	glfwSwapBuffers(main_window);

	past = now;
	return 0;
}

static int load_shaders(void)
{
	shader_sky = complile_simple_program(SHADERS_SKY);
	shader_horse = complile_simple_program(SHADERS_HORSE);
	shader_land = complile_simple_program(SHADERS_LAND);

	return shader_sky == 0 || shader_horse == 0 || shader_land == 0;
}

static int present_and_draw_scene(void)
{
	if (mesh_load(&ship, "res/horse.obj", shader_horse) < 0) {
		log_e("No horse!");
		return -1;
	}

	if (mesh_load(&skybox, "res/skybox.obj", shader_sky) < 0) {
		log_e("No skybox!");
		return -1;
	}

	// if (mesh_load(&cube, "res/cube.obj", SHADERS_SIMPLE) < 0) {
	// 	die("No cube!\n");
	// }

	if (mesh_load(&land, "res/land.obj", shader_land) < 0) {
		log_e("No land!\n");
		return -1;
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

	for (;;) {
		if (main_loop() < 0) {
			break;
		}
	}

	scene_free(&scene);
	mesh_free(&ship);
	mesh_free(&land);
	mesh_free(&skybox);

	return 0;
}

static void on_sigint_handler(int signal)
{
	if (main_window) {
		glfwSetWindowShouldClose(main_window, true);
	}
}

static int init_glfw(void)
{
	int err;

	err = glfwInit();
	if (err < 0) {
		log_e("unable to init glfw!");
		return err;
	}

	return 0;
}

static void window_resize_callback(GLFWwindow* window, int width, int height)
{
	engine_handle_resize(width, height);
}

static int create_main_window(void)
{
	GLenum glew_ret;

	main_window = glfwCreateWindow(640, 480, WINDOW_TITLE, NULL, NULL);
	if (!main_window) {
		log_e("unable to create window handle!");
		return -1;
	}

	glfwMakeContextCurrent(main_window);

	glew_ret = glewInit();
	if (glew_ret != GLEW_OK) {
		log_e("unable to init GLEW! %s", glewGetErrorString(glew_ret));
		glfwDestroyWindow(main_window);
		main_window = NULL;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glfwSetFramebufferSizeCallback(main_window, window_resize_callback);

	log_i("created window with GLEW %s", glewGetString(GLEW_VERSION));
	return 0;
}

static void cleanup_static_libs(void)
{
	glfwTerminate();
}

int main(void)
{
	int err;

	signal(SIGINT, on_sigint_handler);

	err = init_glfw();
	if (err < 0) {
		log_e("unable to init static libraries!");
		return 1;
	}

	err = create_main_window();
	if (err < 0) {
		log_e("unable to create main window!");
		goto cleanup_and_exit;
	}

	err = load_shaders();
	if (err < 0) {
		log_e("unable to load shaders");
		goto cleanup_and_exit;
	}

	log_i("application ready!");
	present_and_draw_scene();

cleanup_and_exit:
	cleanup_static_libs();
	log_i("application exit.");

	return 0;
}
