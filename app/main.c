#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <logger/logger.h>
#include <gl/utils/shaders.h>
#include <engine/engine.h>
#include <engine/scene.h>

#define FLYTHROUGH_CAMERA_IMPLEMENTATION
#include "camera.h"

#define WINDOW_TITLE	"My cool application"

static GLuint shader_sky;
static GLuint shader_horse;
static GLuint shader_land;

static void update_camera(GLFWwindow *window, struct camera *camera,
		float delta_time_sec)
{
	double pos_x, pos_y;
	static double old_pos_x, old_pos_y;
	static const float up[3] = { 0.0f, 1.0f, 0.0f };
	static vec3 camera_position = { 5.0f, 5.0f, 5.0f };
	static vec3 camera_look = { 0.0f, 0.0f, 1.0f };

	glfwGetCursorPos(window, &pos_x, &pos_y);

	flythrough_camera_update(
		camera_position, camera_look, up,
		(float *)camera->view,
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

static int main_loop(GLFWwindow *window, struct camera *camera)
{
	double now; static double past;

	if (glfwWindowShouldClose(window)) {
		return -1;
	}

	now = glfwGetTime();

	update_camera(window, camera, now - past);

	glfwPollEvents();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	engine_redraw();
	glfwSwapBuffers(window);

	past = now;
	return 0;
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
	struct mesh ship, skybox, land;
	struct scene scene;
	struct camera camera;

	if (mesh_load(&ship, "res/horse.obj", shader_horse) < 0) {
		log_e("No horse!");
		return -1;
	}

	if (mesh_load(&skybox, "res/skybox.obj", shader_sky) < 0) {
		log_e("No skybox!");
		return -1;
	}

	if (mesh_load(&land, "res/land.obj", shader_land) < 0) {
		log_e("No land!");
		return -1;
	}

	mat4x4_translate(land.model, 0.0f, -6.0f, 0.0f);

	camera_init(&camera);
	scene_init(&scene);

	scene_add_mesh(&scene, &skybox);
	scene_add_mesh(&scene, &land);
	scene_add_mesh(&scene, &ship);
	// scene_add_mesh(&scene, &cube);

	engine_set_scene(&scene);
	engine_set_camera(&camera);

	for (;;) {
		if (main_loop(window, &camera) < 0) {
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
	engine_handle_resize(width, height);
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

static int do_opengl_stuff(GLFWwindow *window)
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

	return present_and_draw_scene(window);
}

int main(void)
{
	GLFWwindow *window;
	int err;

	err = glfwInit();
	if (err < 0) {
		log_e("unable to init glfw!");
		return 1;
	}

	window = create_context_window();
	if (window) {
		do_opengl_stuff(window);
	} else {
		log_e("unable to create a window!");
	}

	glfwTerminate();
	log_i("we're done");

	return 0;
}
