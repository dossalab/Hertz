#include <utils/glfw_context.h> /* should be the first one */
#include <assets/loader.h>
#include <utils/log.h>
#include <utils/gl/io.h>
#include <render/scene.h>
#include <cameras/fly.h>

#define EXIT_NOT_OK	1

static const char *tag = "main";
static const char *window_title = "My cool application";

struct render_state {
	struct scene scene;
	struct fly_camera camera;
	double time;
	GLuint shader_sky;
	GLuint shader_horse;
	GLuint shader_land;
};

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
	camera_update_perspective(&state->camera.as_camera, aspect);
}

static void glfw_on_draw(GLFWwindow *window, double spent, void *user)
{
	struct render_state *state = user;

	camera_update(&state->camera.as_camera, window, spent);
	scene_update_mvp(&state->scene, state->camera.as_camera.vp);

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

	camera_init(&state->camera.as_camera, &fly_camera_proto);

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

int main(void)
{
	bool ok;
	struct render_state state;

	logger_init(LOGLEVEL_INFO);

	struct glfw_ctx_callbacks callbacks = {
		.on_init = glfw_on_init,
		.on_draw = glfw_on_draw,
		.on_resize = glfw_on_resize,
		.on_exit = glfw_on_exit,
		.user = &state,
	};

	ok = glfw_ctx_main(window_title, &callbacks);
	if (!ok) {
		log_e(tag, "render finished with an error");
	}

	log_i(tag, "we're done");

	return ok? 0 : EXIT_NOT_OK;
}
