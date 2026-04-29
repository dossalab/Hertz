#include <hz/hertz.h>
#include <hz/built-in/shaders/phong.h>
#include <hz/contexts/glfw.h>
#include <hz/actors/fly.h>
#include <stdio.h>

#include "assimp.h"

static const char *tag = "main";
static const char *window_title = "My cool application";

struct render_state {
	const char *scene_path;
	hz_program *program;
	hz_fly_actor *actor;
	float last_x, last_y;
};

static bool load_shaders(struct render_state *state, hz_arena *arena)
{
	state->program = hz_phong_program_new(arena);

	return !!state->program;
}

static void update(hz_context *context, float dt, void *user)
{
	struct render_state *s = user;
	float x, y;

	hz_context_get_cursor(context, &x, &y);

	hz_fly_actor_move(s->actor,
		dt,
		x - s->last_x,
		y - s->last_y,
		hz_context_get_key(context, 'w'),
		hz_context_get_key(context, 'a'),
		hz_context_get_key(context, 's'),
		hz_context_get_key(context, 'd'));

	s->last_x = x;
	s->last_y = y;
}

static bool init(hz_context *context, hz_arena *arena, hz_node *root, void *user)
{
	struct render_state *state = user;
	hz_node *camera, *light;

	hz_glfw_context_set_window_title(HZ_GLFW_CONTEXT(context), window_title);

	if (!load_shaders(state, arena)) {
		hz_log_e(tag, "unable to load shaders!");
		return false;
	}

	camera = hz_camera_new(arena);
	hz_node_move(camera, (hz_vec3) { 0.f, 0.f, 4.f });
	hz_node_insert(root, camera);
	hz_context_set_camera(context, camera);

	state->actor = hz_fly_actor_new(arena, camera);

	light = hz_light_new(arena, state->program, 0);
	hz_node_move(light, (hz_vec3) { 0.f, 10.f, 0.f });
	hz_light_setup(HZ_LIGHT(light), 0.1f, 0.06f, 0.001f);
	hz_node_insert(root, light);

	if (!assimp_import_scene(root, arena, state->scene_path, state->program)) {
		hz_log_e(tag, "unable to import scene!");
		return false;
	}

	return true;
}

int main(int argc, char **argv)
{
	struct render_state state = {0};

	if (argc != 2) {
		printf("usage: %s <scene>\n", argv[0]);
		return 0;
	}

	hz_logger_init(HZ_LOGLEVEL_DEBUG);
	state.scene_path = argv[1];

	return hz_context_wrapper(hz_glfw_context_new, init, update, &state);
}
