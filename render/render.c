#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <utils/glfw_context.h> /* This should go before other GL includes */
#include <utils/gl/shaders.h>
#include <utils/gl/textures.h>
#include <utils/gl/io.h>
#include <utils/log.h>
#include <utils/files.h>
#include <utils/3rdparty/linmath/linmath.h>
#include <cameras/fly_camera.h>
#include <assets/loader.h>

#include "render.h"
#include "scene.h"

#define ENG_PI		3.14159265359f
#define ENG_FOV		(70.0f * ENG_PI / 180.0f)

static const char *tag = "render";
int assimp_shader;

static bool load_shaders(struct render_state *state)
{
	state->shader_sky = create_program_from_files("shaders/simple.vert",
			"shaders/skybox.frag");
	state->shader_horse = create_program_from_files("shaders/psycho.vert",
			"shaders/simple.frag");
	state->shader_land = create_program_from_files("shaders/simple.vert",
			"shaders/simple.frag");

	assimp_shader = state->shader_land;

	return state->shader_sky && state->shader_horse && state->shader_land;
}

static void glfw_on_resize(GLFWwindow *window, size_t w, size_t h, void *user)
{
	float aspect;
	struct render_state *state = user;

	aspect = (float)w / (float)h;

	glViewport(0, 0, w, h);
	fly_camera_update_projection(&state->fly_camera, ENG_FOV, aspect);
}

static void glfw_on_draw(GLFWwindow *window, double spent, void *user)
{
	mat4x4 identity;
	struct render_state *state = user;

	mat4x4_identity(identity);

	fly_camera_update(&state->fly_camera, window, spent);
	scene_update_mvp(&state->scene, state->fly_camera.vp);
	state->time += spent;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene_redraw(&state->scene, state->time);
}

static void glfw_on_exit(GLFWwindow *window, void *user)
{
	/* pass */
}

static bool glfw_on_init(GLFWwindow *window, void *user)
{
	bool ok;
	struct render_state *state = user;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	fly_camera_reset(&state->fly_camera);

	/* TODO: exit path cleanups */
	ok = load_shaders(user);
	if (!ok) {
		log_e(tag, "unable to load shaders!");
		return false;
	}

	ok = loader_import_scene("res/scene.glb", &state->scene);
	if (!ok) {
		log_e(tag, "unable to import scene");
		return false;
	}

	return true;
}

bool render_init(struct render_state *state)
{
	memset(state, 0, sizeof(struct render_state));

	state->glfw_callbacks.on_init = glfw_on_init;
	state->glfw_callbacks.on_draw = glfw_on_draw;
	state->glfw_callbacks.on_resize = glfw_on_resize;
	state->glfw_callbacks.on_exit = glfw_on_exit;
	state->glfw_callbacks.user = state;

	return true;
}

void render_exit(struct render_state *state)
{
	/* pass */
}
