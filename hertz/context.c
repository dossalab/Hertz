#include <hz/nodes/root.h>
#include <hz/nodes/light.h>
#include <hz/internal/context.h>
#include <hz/context.h>
#include <hz/util/logger.h>

static const char *tag = "ctx";

#define SUN_LIGHT_INDEX		0
#define SUN_POSITION		(hz_vec3) { 0.f, 10.f, 0.f }

/* Light source parameters */
#define SUN_CONSTANT		0.1
#define SUN_LINEAR		0.06
#define SUN_QUADRATIC		0.01

void hz_context_handle_resize(hz_context *context, unsigned w, unsigned h)
{
	glViewport(0, 0, w, h);
	hz_camera_resize(HZ_CAMERA(context->camera), w, h);
}

bool hz_context_get_key(hz_context *context, char key)
{
	return context->proto->get_key(context, key);
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
		hz_context_wrapper_init init, void *user)
{
	hz_node *root, *sun;
	int shader;

	root = hz_root_new(arena);

	if (!init(context, arena, root, &shader, user)) {
		hz_log_e(tag, "user init failed");
		return HZ_EXIT_CODE_ERROR;
	}

	context->camera = hz_camera_new(arena, shader);
	hz_node_insert(root, context->camera);

	sun = hz_light_new(arena, shader, SUN_LIGHT_INDEX);

	hz_node_move(sun, SUN_POSITION);
	hz_light_setup(HZ_LIGHT(sun), SUN_CONSTANT, SUN_LINEAR, SUN_QUADRATIC);

	hz_node_insert(root, sun);

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while (hz_context_poll(context)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		hz_node_draw(root, HZ_CAMERA(context->camera));
	}

	return HZ_EXIT_CODE_OK;
}

int hz_context_wrapper_with_arena(hz_arena *arena, hz_context_constructor constructor,
		hz_context_wrapper_init init, void *user)
{
	int ret;
	hz_context *context;

	context = constructor(arena);
	if (!context) {
		hz_log_e(tag, "unable to create given context");
		return HZ_EXIT_CODE_ERROR;
	}

	ret = hz_context_wrapper_with_context(arena, context, init, user);

	hz_context_exit(context);
	hz_log_i(tag, "context is done");

	return ret;
}

int hz_context_wrapper(hz_context_constructor constructor, hz_context_wrapper_init init, void *user)
{
	int ret;
	hz_arena *arena;

	arena = hz_arena_new();
	if (!arena) {
		hz_log_e(tag, "unable to allocate arena");
		return HZ_EXIT_CODE_ERROR;
	}

	ret = hz_context_wrapper_with_arena(arena, constructor, init, user);

	hz_arena_free(arena);
	hz_log_d(tag, "context arena is cleared");

	return ret;
}
