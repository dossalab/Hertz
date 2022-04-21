#include "glfw_context.h" /* should be the first one */
#include <stdio.h>
#include <stdlib.h>

#include <hz/cameras/fly.h>
#include <hz/logger.h>
#include <hz/scene.h>
#include <hz/loader.h>
#include <hz/helpers/shaders.h>
#include <hz/built-in/shaders/simple.h>

#define EXIT_NOT_OK	1

static const char *tag = "main";
static const char *window_title = "My cool application";

struct render_state {
	struct hz_scene scene;
	struct hz_fly_camera camera;
	const char *scene_path;
	double time;
	GLuint shader;
};

int assimp_shader;

static bool load_shaders(struct render_state *state)
{
	char *logs;

	state->shader = hz_create_program_from_source(
				hz_simple_vertex_shader_source,
				hz_simple_fragment_shader_source, &logs);

	if (!state->shader) {
		hz_log_e(tag, "shader program compilation failed!");
		if (logs) {
			hz_log_e(tag, "compilation logs:\n%s", logs);
			free(logs);
		}

		return false;
	}

	assimp_shader = state->shader;
	return true;
}

static void camera_update(struct render_state *s, GLFWwindow *window, float spent)
{
	double pos_x, pos_y;
	static double old_pos_x, old_pos_y;

	glfwGetCursorPos(window, &pos_x, &pos_y);

	fly_camera_move(&s->camera,
		spent,
		pos_x - old_pos_x,
		pos_y - old_pos_y,
		glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS,
		glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS,
		glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS,
		glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);

	old_pos_x = pos_x;
	old_pos_y = pos_y;
}


static void glfw_on_resize(GLFWwindow *window, size_t w, size_t h, void *user)
{
	struct render_state *state = user;

	glViewport(0, 0, w, h);
	hz_camera_update(hz_cast_camera(&state->camera), w, h);
}

static void glfw_on_draw(GLFWwindow *window, double spent, void *user)
{
	struct render_state *state = user;

	camera_update(state, window, spent);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	hz_scene_draw(&state->scene, hz_cast_camera(&state->camera), state->time);
}

static void glfw_on_exit(GLFWwindow *window, void *user)
{
	/* pass */
}

static bool glfw_on_init(GLFWwindow *window, void *user)
{
	bool ok;
	struct render_state *state = user;

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	hz_camera_init(hz_cast_camera(&state->camera), &hz_fly_camera_proto);

	/* TODO: exit path cleanups */
	ok = load_shaders(user);
	if (!ok) {
		hz_log_e(tag, "unable to load shaders!");
		return false;
	}

	ok = hz_loader_import_scene(state->scene_path, &state->scene);
	if (!ok) {
		hz_log_e(tag, "unable to import scene");
		return false;
	}

	return true;
}

int main(int argc, char **argv)
{
	bool ok;
	struct render_state state;

	if (argc != 2) {
		printf("usage: %s <scene>\n", argv[0]);
		return EXIT_NOT_OK;
	}

	hz_logger_init(HZ_LOGLEVEL_INFO);
	state.scene_path = argv[1];

	struct glfw_ctx_callbacks callbacks = {
		.on_init = glfw_on_init,
		.on_draw = glfw_on_draw,
		.on_resize = glfw_on_resize,
		.on_exit = glfw_on_exit,
		.user = &state,
	};

	ok = glfw_ctx_main(window_title, &callbacks);
	if (!ok) {
		hz_log_e(tag, "render finished with an error");
	}

	hz_log_i(tag, "we're done");

	return ok? 0 : EXIT_NOT_OK;
}
