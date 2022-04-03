#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <utils/log.h>
#include <utils/gl/shaders.h>
#include <utils/gl/textures.h>
#include <scene/scene.h>
#include <utils/linmath.h>
#include <cameras/fly_camera.h>
#include <loaders/obj_loader.h>
#include <loaders/stb_image.h>
#include <utils/files.h>
#include <utils/glfw_ctx_helper.h>

#define ENG_PI		3.14159265359f
#define ENG_FOV		(70.0f * ENG_PI / 180.0f)
#define WINDOW_TITLE	"My cool application"

static GLuint shader_sky;
static GLuint shader_horse;
static GLuint shader_land;

struct render_state {
	struct mesh ship, skybox, land, wall;
	struct camera fly_camera;
	struct scene scene;
	double time;
};

GLuint load_shader_from_file(const char *filename, GLenum type)
{
	GLuint shader;
	char *source;
	char *compilation_logs;

	source = read_text_file(filename);
	if (!source) {
		log_e("unable to read shader %s", filename);
		return 0;
	}

	shader = compile_single_shader(source, &compilation_logs, type);
	free(source);

	if (!shader) {
		if (compilation_logs) {
			log_i("shader compilation logs:\n%s", compilation_logs);
			free(compilation_logs);
		}
	}

	return shader;
}

static GLuint compile_simple_program(const char *vert_path, const char *frag_path)
{
	GLuint vertex, frag;
	GLuint program = 0;

	vertex = load_shader_from_file(vert_path, GL_VERTEX_SHADER);
	if (!vertex) {
		log_e("unable to load vertex shader (%s)", vert_path);
		return 0;
	}

	frag = load_shader_from_file(frag_path, GL_FRAGMENT_SHADER);
	if (!frag) {
		log_e("unable to load fragment shader (%s)", frag_path);
		glDeleteShader(vertex);
		return 0;
	}

	program = create_shader_program(2, vertex, frag);
	if (!program) {
		log_e("unable to create program!");

		glDeleteShader(vertex);
		glDeleteShader(frag);
		return 0;
	}

	return program;
}

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

static bool load_shaders(void)
{
	shader_sky = compile_simple_program("shaders/simple.vert",
			"shaders/skybox.frag");
	shader_horse = compile_simple_program("shaders/psycho.vert",
			"shaders/simple.frag");
	shader_land = compile_simple_program("shaders/simple.vert",
			"shaders/simple.frag");

	if (shader_sky == 0 || shader_horse == 0 || shader_land == 0) {
		return false;
	}

	return true;
}

static bool load_assets(struct render_state *state)
{
	bool ok;

	fly_camera_reset(&state->fly_camera);

	ok = load_mesh_from_obj(&state->ship, "res/horse.obj", shader_horse);
	if (!ok) {
		log_e("No horse!");
		return false;
	}

	ok = load_mesh_from_obj(&state->skybox, "res/skybox.obj", shader_sky);
	if (!ok) {
		log_e("No skybox!");
		return false;
	}

	ok = load_mesh_from_obj(&state->land, "res/land.obj", shader_land);
	if (!ok) {
		log_e("No land!");
		return false;
	}

	ok = load_mesh_from_obj(&state->wall, "res/wall.obj", shader_land);
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
	ok = load_shaders();
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

int main(void)
{
	bool ret;
	struct render_state state;

	memset(&state, 0, sizeof(state));

	struct glfw_ctx_callbacks callbacks = {
		.on_init = glfw_on_init,
		.on_draw = glfw_on_draw,
		.on_resize = glfw_on_resize,
		.on_exit = glfw_on_exit,

		.user = &state,
	};

	ret = glfw_ctx_main(WINDOW_TITLE, &callbacks);
	if (!ret) {
		log_e("context helper exited with an error");
	}

	return 0;
}
