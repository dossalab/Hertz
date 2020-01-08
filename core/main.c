#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef BUILD_WEBGL
#include <emscripten/emscripten.h>
#endif

#include <engine/api.h>

static struct camera main_camera;
static struct scene scene;
static struct mesh ship, skybox;

static void die(const char *fmt, ...)
{
	va_list params;

	va_start(params, fmt);
	vfprintf(stderr, fmt, params);
	va_end(params);

	#ifdef BUILD_WEBGL
	        emscripten_force_exit(EXIT_FAILURE);
	#else
		exit(1);
	#endif
}

static void on_mouse(struct event_data *event)
{
	mat4x4_identity(main_camera.view);

	mat4x4_rotate(
		main_camera.view,
		main_camera.view,
		1,
		0,
		0,
		event->mouse.y * ENG_PI / 2.0
	);

	mat4x4_rotate(
		main_camera.view,
		main_camera.view,
		0,
		1,
		0,
		event->mouse.x * ENG_PI / 2.0
	);

	mat4x4_translate_in_place(main_camera.view, 0, -20, 0);
	engine_update_mvp();
}

static void on_keyboard(struct event_data *event)
{
}

static void main_loop(void)
{
	if (engine_poll() < 0) {
		// ?
	}
}

int main(int argc, char **argv)
{
	if (engine_init() < 0) {
		die("Unable to init engine!\n");
	}

	if (mesh_load(&ship, "res/ship.stl") < 0) {
		die("No ship!\n");
	}

	if (mesh_load(&skybox, "res/skybox.stl") < 0) {
		die("No skybox!\n");
	}

	camera_init(&main_camera);
	scene_init(&scene);

	scene_add_mesh(&scene, &skybox);
	scene_add_mesh(&scene, &ship);

	engine_set_scene(&scene);
	engine_set_camera(&main_camera);

	engine_subscribe(ENG_EVENT_MOUSE, on_mouse);
	engine_subscribe(ENG_EVENT_KEYBOARD, on_keyboard);

	#ifdef BUILD_WEBGL
		emscripten_set_main_loop(main_loop, 0, 0);
	#else
		for (;;) {
			main_loop();
		}

		// should never appear here?
		scene_free(&scene);
		mesh_free(&ship);
		mesh_free(&skybox);
	#endif

	return 0;
}

