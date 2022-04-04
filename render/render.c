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
#include <utils/linmath.h>
#include <cameras/fly_camera.h>
#include <loaders/obj_loader.h>
#include <loaders/stb_image.h>

#include "render.h"
#include "scene.h"

#define ENG_PI		3.14159265359f
#define ENG_FOV		(70.0f * ENG_PI / 180.0f)

static GLuint create_texture_from_file(const char *path)
{
	int w, h, n;
	uint8_t *data;
	GLuint texture;

	data = stbi_load(path, &w, &h, &n, 3);
	if (!data) {
		return 0;
	}

	texture = create_texture_from_rgb(data, w, h);
	stbi_image_free(data);

	return texture;
}

static bool load_mesh_from_obj(struct mesh *m, char *path, GLuint shader_program)
{
	bool ok;
	struct model model;
	GLuint texture;

	ok = loader_load_obj(&model, path);
	if (!ok) {
		log_e("unable to load obj file '%s'", path);
		return false;
	}

	ok = mesh_create_from_geometry(m, shader_program, model.vertices,
			model.vertex_count, model.normals, model.normal_count);
	if (!ok) {
		log_e("unable to create mesh");
		goto exit;
	}

	if (model.texture_path) {
		texture = create_texture_from_file(model.texture_path);
		if (!texture) {
			ok = false;
			goto exit;
		}

		ok = mesh_texture(m, texture, model.uvs, model.uv_count);
	}

exit:
	free_model(&model);
	return ok;
}

static bool load_shaders(struct render_state *state)
{
	state->shader_sky = create_program_from_files("shaders/simple.vert",
			"shaders/skybox.frag");
	state->shader_horse = create_program_from_files("shaders/psycho.vert",
			"shaders/simple.frag");
	state->shader_land = create_program_from_files("shaders/simple.vert",
			"shaders/simple.frag");

	return state->shader_sky && state->shader_horse && state->shader_land;
}

static bool load_assets(struct render_state *state)
{
	bool ok;

	fly_camera_reset(&state->fly_camera);

	ok = load_mesh_from_obj(&state->ship, "res/horse.obj", state->shader_horse);
	if (!ok) {
		log_e("No horse!");
		return false;
	}

	ok = load_mesh_from_obj(&state->skybox, "res/skybox.obj", state->shader_sky);
	if (!ok) {
		log_e("No skybox!");
		return false;
	}

	ok = load_mesh_from_obj(&state->land, "res/land.obj", state->shader_land);
	if (!ok) {
		log_e("No land!");
		return false;
	}

	ok = load_mesh_from_obj(&state->wall, "res/wall.obj", state->shader_land);
	if (!ok) {
		log_e("No wall!");
		return false;
	}

	mat4x4_translate(state->land.model, 0.0f, -6.0f, 0.0f);
	scene_init(&state->scene);

	scene_add_mesh(&state->scene, &state->skybox);
	scene_add_mesh(&state->scene, &state->land);
	scene_add_mesh(&state->scene, &state->ship);
	scene_add_mesh(&state->scene, &state->wall);

	return true;
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

	/* 1 rad / sec */
	mat4x4_rotate_Y(state->ship.model, identity, 2 * ENG_PI * state->time);
	mat4x4_translate(state->wall.model, 10.0f, state->time, -10.0f);

	fly_camera_update(&state->fly_camera, window, spent);
	scene_update_mvp(&state->scene, state->fly_camera.vp);
	state->time += spent;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene_redraw(&state->scene, state->time);
}

static void glfw_on_exit(GLFWwindow *window, void *user)
{
	struct render_state *state = user;

	scene_free(&state->scene);
	mesh_free(&state->ship);
	mesh_free(&state->wall);
	mesh_free(&state->land);
	mesh_free(&state->skybox);
}

static bool glfw_on_init(GLFWwindow *window, void *user)
{
	bool ok;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	/* TODO: exit path cleanups */
	ok = load_shaders(user);
	if (!ok) {
		log_e("unable to load shaders!");
		return false;
	}

	ok = load_assets(user);
	if (!ok) {
		log_e("unable to load all assets");
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
