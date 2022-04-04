#include <utils/glfw_context.h> /* should be the first one */
#include <utils/log.h>
#include <render/render.h>

#define EXIT_NOT_OK	1

static const char *window_title = "My cool application";

int main(void)
{
	bool ok;
	struct render_state state;

	ok = render_init(&state);
	if (!ok) {
		log_e("unable to init render");
		return EXIT_NOT_OK;
	}

	ok = glfw_ctx_main(window_title, &state.glfw_callbacks);
	if (!ok) {
		log_e("render finished with an error");
	}

	render_exit(&state);
	return ok? 0 : EXIT_NOT_OK;
}
