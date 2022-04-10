#include <utils/glfw_context.h> /* should be the first one */
#include <utils/log.h>
#include <render/render.h>

#define EXIT_NOT_OK	1

static const char *tag = "main";
static const char *window_title = "My cool application";

int main(void)
{
	bool ok;
	struct render_state state;

	logger_init(LOGLEVEL_INFO);

	ok = render_init(&state);
	if (!ok) {
		log_e(tag, "unable to init render");
		return EXIT_NOT_OK;
	}

	ok = glfw_ctx_main(window_title, &state.glfw_callbacks);
	if (!ok) {
		log_e(tag, "render finished with an error");
	}

	render_exit(&state);
	log_i(tag, "we're done");

	return ok? 0 : EXIT_NOT_OK;
}
