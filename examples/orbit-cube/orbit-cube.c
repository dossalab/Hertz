#include <hz/hertz.h>
#include <hz/built-in/shaders/phong.h>
#include <hz/built-in/shaders/wireframe.h>
#include <hz/built-in/primitives/cube.h>
#include <hz/built-in/materials/checkerboard.h>
#include <hz/contexts/glfw.h>
#include <hz/actors/orbit.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

struct state {
	hz_orbit_actor *actor;
	double last_x, last_y;
	bool prev_pressed;
	bool wireframe;
};

static void update(hz_context *context, float dt, void *user)
{
	struct state *s = user;
	float x, y;
	bool pressed;

	hz_context_get_cursor(context, &x, &y);
	pressed = hz_context_get_mouse_button(context, HZ_MOUSE_BUTTON_LEFT);

	if (pressed && s->prev_pressed) {
		hz_orbit_actor_rotate(s->actor, x - s->last_x, y - s->last_y);
	}

	s->last_x = x;
	s->last_y = y;
	s->prev_pressed = pressed;
}

static bool init(hz_context *context, hz_arena *arena, hz_node *root, void *user)
{
	struct state *s = user;
	hz_program *program;
	hz_node *camera, *light;

	hz_glfw_context_set_window_title(HZ_GLFW_CONTEXT(context), "Orbit cube - drag to rotate");

	program = s->wireframe
		? hz_wireframe_program_new(arena)
		: hz_phong_program_new(arena);

	if (!program) {
		return false;
	}

	camera = hz_camera_new(arena);
	hz_node_move(camera, (hz_vec3) { 0.f, 0.f, 4.f });
	hz_node_insert(root, camera);
	hz_context_set_camera(context, camera);

	s->actor = hz_orbit_actor_new(arena, camera);

	light = hz_light_new(arena, program, 0);
	hz_node_move(light, (hz_vec3) { 0.f, 10.f, 0.f });
	hz_light_setup(HZ_LIGHT(light), 0.1f, 0.06f, 0.001f);
	hz_node_insert(root, light);

	hz_material *material = hz_checkerboard_material_new(arena, 8);
	hz_node *cube = hz_cube_new(arena, program, material);
	hz_node_insert(root, cube);

	return true;
}

int main(int argc, char **argv)
{
	struct state state = {0};
	int opt;

	while ((opt = getopt(argc, argv, "s:")) != -1) {
		switch (opt) {
		case 's':
			if (strcmp(optarg, "wireframe") == 0) {
				state.wireframe = true;
			} else if (strcmp(optarg, "phong") != 0) {
				fprintf(stderr, "unknown shader '%s', use: phong, wireframe\n", optarg);
				return 1;
			}
			break;
		default:
			fprintf(stderr, "usage: orbit-cube [-s phong|wireframe]\n");
			return 1;
		}
	}

	hz_logger_init(HZ_LOGLEVEL_DEBUG);
	return hz_context_wrapper(hz_glfw_context_new, init, update, &state);
}
