#include <engine/engine.h>
#include <engine/events.h>

static void (*handlers[ENG_EVENT_COUNT])(struct event_data *);

static void cursor_callback(GLFWwindow *window, double x, double y)
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	struct event_data data = {
		.type = ENG_EVENT_MOUSE,
		.mouse = {
			.x = (x / ((double)width / 2.0) - 1),
			.y = (y / ((double)height / 2.0) - 1)
		}
	};

	handlers[data.type](&data);
}

static void key_callback(GLFWwindow *window, int key, int code, int act, int mod)
{
	struct event_data data = {
		.type = ENG_EVENT_KEYBOARD,
		.keyboard = {
			.key = key,
			.mod = mod,
			.action = act
		}
	};

	handlers[data.type](&data);
}

void engine_subscribe(int event, void (*handler)(struct event_data *))
{
	GLFWwindow *window = engine_get_window();

	switch (event) {
	case ENG_EVENT_MOUSE:
		handlers[ENG_EVENT_MOUSE] = handler;
		glfwSetCursorPosCallback(window, cursor_callback);
		break;

	case ENG_EVENT_KEYBOARD:
		handlers[ENG_EVENT_KEYBOARD] = handler;
		glfwSetKeyCallback(window, key_callback);
		break;
	}
}

void engine_events_init(void)
{
	/* pass */
}

