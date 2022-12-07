#include <stdio.h>
#include <stdlib.h>

#include <hz/actors/fly.h>
#include <hz/util/logger.h>
#include <hz/helpers/shaders.h>
#include <hz/nodes/light.h>
#include <hz/nodes/root.h>
#include <hz/nodes/camera.h>
#include <hz/built-in/shaders/simple.h>
#include <hz/contexts/glfw.h>

#include "assimp.h"

static const char *tag = "main";
static const char *window_title = "My cool application";

struct render_state {
	const char *scene_path;
	GLuint shader;
};

int assimp_shader;

static bool load_shaders(struct render_state *state, int *shader)
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
	*shader = state->shader;
	return true;
}

// static void camera_update(struct render_state *s, GLFWwindow *window, float spent)
// {
// 	double pos_x, pos_y;
// 	static double old_pos_x, old_pos_y;

// 	glfwGetCursorPos(window, &pos_x, &pos_y);

// 	hz_fly_actor_move(s->actor,
// 		spent,
// 		pos_x - old_pos_x,
// 		pos_y - old_pos_y,
// 		hz_context_get_key(context, 'w'),
// 		hz_context_get_key(context, 'a'),
// 		hz_context_get_key(context, 's'),
// 		hz_context_get_key(context, 'd'));

// 	old_pos_x = pos_x;
// 	old_pos_y = pos_y;
// }

static bool init(hz_context *context, hz_arena *arena, hz_node *root, int *shader, void *user)
{
	bool ok;
	struct render_state *state = user;

	hz_glfw_context_set_window_title(HZ_GLFW_CONTEXT(context), window_title);

	ok = load_shaders(user, shader);
	if (!ok) {
		hz_log_e(tag, "unable to load shaders!");
		return false;
	}

	if (!assimp_import_scene(root, arena, state->scene_path)) {
		hz_log_e(tag, "unable to import scene!");
		return false;
	}

	return true;
}

int main(int argc, char **argv)
{
	struct render_state state;

	if (argc != 2) {
		printf("usage: %s <scene>\n", argv[0]);
		return 0;
	}

	hz_logger_init(HZ_LOGLEVEL_DEBUG);
	state.scene_path = argv[1];

	return hz_context_wrapper(hz_glfw_context_new, init, &state);
}
