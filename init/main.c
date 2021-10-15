#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <logger/logger.h>
#include <gl/shaders.h>
#include <scene/scene.h>
#include <utils/linmath.h>
#include <cameras/fly_camera.h>
#include <errors/errors.h>
#include <counters/time.h>

#define ENG_PI		3.14159265359f
#define ENG_FOV		(70.0f * ENG_PI / 180.0f)
#define WINDOW_TITLE	"My cool application"

static GLuint shader_sky;
static GLuint shader_horse;
static GLuint shader_land;

static bool main_loop(GLFWwindow *window, struct scene *scene, struct camera *c)
{
	double now, spent; static double past;

	if (glfwWindowShouldClose(window)) {
		return false;
	}

	now = glfwGetTime();
	spent = now - past;

	fly_camera_update(c, window, spent);
	scene_update_mvp(scene, c->vp);
	update_counter(spent);

	glfwPollEvents();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene_redraw(scene);
	glfwSwapBuffers(window);

	past = now;
	return true;
}

static int load_shaders(void)
{
	shader_sky = compile_simple_program("shaders/simple.vert",
			"shaders/skybox.frag");
	shader_horse = compile_simple_program("shaders/psycho.vert",
			"shaders/simple.frag");
	shader_land = compile_simple_program("shaders/simple.vert",
			"shaders/simple.frag");

	if (shader_sky == 0 || shader_horse == 0 || shader_land == 0) {
		return -ERR_SHADER_INVALID;
	}

	return 0;
}

static int present_and_draw_scene(GLFWwindow *window)
{
	int err;

	struct mesh ship, skybox, land, wall;
	struct camera fly_camera;
	struct scene scene;

	fly_camera_reset(&fly_camera);
	glfwSetWindowUserPointer(window, &fly_camera);

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

	err = mesh_load(&wall, "res/wall.obj", shader_land);
	if (err < 0) {
		log_e("No wall!");
		return err;
	}

	mat4x4_translate(land.model, 0.0f, -6.0f, 0.0f);
	scene_init(&scene);

	scene_add_mesh(&scene, &skybox);
	scene_add_mesh(&scene, &land);
	scene_add_mesh(&scene, &ship);
	scene_add_mesh(&scene, &wall);

	for (;;) {
		mat4x4 identity;
		mat4x4_identity(identity);

		/* 1 rad / sec */
		mat4x4_rotate_Y(ship.model, identity, 2 * ENG_PI * global_time_counter);
		mat4x4_translate(wall.model, 10.0f, global_time_counter, -10.0f);


		if (!main_loop(window, &scene, &fly_camera)) {
			break;
		}
	}

	scene_free(&scene);
	mesh_free(&ship);
	mesh_free(&wall);
	mesh_free(&land);
	mesh_free(&skybox);

	return 0;
}

static void window_resize_callback(GLFWwindow* window, int width, int height)
{
	float aspect;
	struct camera *c;

	aspect = (float)width / (float)height;

	glViewport(0, 0, width, height);
	c = glfwGetWindowUserPointer(window);
	fly_camera_update_projection(c, ENG_FOV, aspect);
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

	log_d("time counter value is %f s", global_time_counter);
	return 0;
}
