#include HZ_GL_EXTENSIONS_HEADER
#include <time.h>
#include <hz/nodes/root.h>
#include <hz/internal/context.h>
#include <hz/context.h>
#include <hz/material_store.h>
#include <hz/util/logger.h>

static const char *tag = "ctx";

void hz_context_handle_resize(hz_context *context, unsigned w, unsigned h)
{
	glViewport(0, 0, w, h);
	hz_camera_resize(HZ_CAMERA(context->camera), w, h);
}

bool hz_context_get_key(hz_context *context, char key)
{
	return context->proto->get_key(context, key);
}

bool hz_context_get_mouse_button(hz_context *context, int button)
{
	return context->proto->get_mouse_button(context, button);
}

void hz_context_get_cursor(hz_context *context, float *x, float *y)
{
	context->proto->get_cursor(context, x, y);
}

float hz_context_get_scroll(hz_context *context)
{
	return context->proto->get_scroll(context);
}

hz_node *hz_context_get_camera(hz_context *context)
{
	return context->camera;
}

void hz_context_set_camera(hz_context *context, hz_node *camera)
{
	context->camera = camera;
}

hz_material_store *hz_context_get_material_store(hz_context *context)
{
	return context->store;
}

bool hz_context_poll(hz_context *context)
{
	return context->proto->poll(context);
}

void hz_context_exit(hz_context *context)
{
	context->proto->exit(context);
}

void hz_context_init(hz_context *context, const hz_context_proto *proto)
{
	context->proto = proto;
}

int hz_context_wrapper_with_context(hz_arena *arena, hz_context *context,
		hz_context_wrapper_init init, hz_context_update_fn update, void *user)
{
	hz_node *root;

	root = hz_root_new(arena);
	context->store = hz_material_store_new(arena);

	if (!init(context, arena, root, user)) {
		hz_log_e(tag, "user init failed");
		return HZ_EXIT_CODE_ERROR;
	}

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	struct timespec prev, now;
	clock_gettime(CLOCK_MONOTONIC, &prev);

	while (hz_context_poll(context)) {
		float dt;

		clock_gettime(CLOCK_MONOTONIC, &now);
		dt = (now.tv_sec - prev.tv_sec) + (now.tv_nsec - prev.tv_nsec) * 1e-9f;
		prev = now;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (update)
			update(context, dt, user);
		hz_node_draw(root, HZ_CAMERA(context->camera));
	}

	return HZ_EXIT_CODE_OK;
}

int hz_context_wrapper_with_arena(hz_arena *arena, hz_context_constructor constructor,
		hz_context_wrapper_init init, hz_context_update_fn update, void *user)
{
	int ret;
	hz_context *context;

	context = constructor(arena);
	if (!context) {
		hz_log_e(tag, "unable to create given context");
		return HZ_EXIT_CODE_ERROR;
	}

	ret = hz_context_wrapper_with_context(arena, context, init, update, user);

	hz_context_exit(context);
	hz_log_i(tag, "context is done");

	return ret;
}

int hz_context_wrapper(hz_context_constructor constructor,
		hz_context_wrapper_init init, hz_context_update_fn update, void *user)
{
	int ret;
	hz_arena *arena;

	arena = hz_arena_new();
	if (!arena) {
		hz_log_e(tag, "unable to allocate arena");
		return HZ_EXIT_CODE_ERROR;
	}

	ret = hz_context_wrapper_with_arena(arena, constructor, init, update, user);

	hz_arena_free(arena);
	hz_log_d(tag, "context arena is cleared");

	return ret;
}
