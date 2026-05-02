#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <ctype.h>
#include <hz/internal/context.h>
#include <hz/util/logger.h>
#include <hz/contexts/glfw.h>
#include <hz/util/common.h>
#include <hz/nodes/camera.h>

const char *tag = "glfw";

static unsigned glfw_init_refcounter = 0;

struct _hz_glfw_context {
	hz_context super;
	GLFWwindow *window;
};

static void glfw_resize_callback(GLFWwindow *window, int w, int h)
{
	hz_context *super = glfwGetWindowUserPointer(window);
	hz_context_handle_resize(super, w, h);
}

static GLFWwindow *create_context_window(void)
{
	GLFWwindow *window;

	window = glfwCreateWindow(640, 480, "", NULL, NULL);
	if (!window) {
		hz_log_e(tag, "unable to create window handle!");
		return NULL;
	}

	glfwMakeContextCurrent(window);

	/*
	 * uhh we have to do that *after* context is created.
	 * not sure what will happen if we create multiple windows
	 */
	if (!gladLoadGL(glfwGetProcAddress)) {
		hz_log_e(tag, "unable to init GLAD!");
		glfwDestroyWindow(window);
		return NULL;
	}

	hz_log_i(tag, "created window with OpenGL %s", glGetString(GL_VERSION));
	return window;
}

static void glfw_exit(hz_context *super)
{
	hz_glfw_context *context = HZ_GLFW_CONTEXT(super);

	glfwDestroyWindow(context->window);

	if (glfw_init_refcounter == 0) {
		glfwTerminate();
	}
}

static bool glfw_poll(hz_context *super)
{
	hz_glfw_context *context = HZ_GLFW_CONTEXT(super);

	glfwPollEvents();
	glfwSwapBuffers(context->window);

	return !glfwWindowShouldClose(context->window);
}

static bool glfw_get_mouse_button(hz_context *super, int button)
{
	hz_glfw_context *context = HZ_GLFW_CONTEXT(super);
	return glfwGetMouseButton(context->window, button) == GLFW_PRESS;
}

static void glfw_get_cursor(hz_context *super, float *x, float *y)
{
	double _x, _y; // GLFW expects doubles here
	hz_glfw_context *context = HZ_GLFW_CONTEXT(super);

	glfwGetCursorPos(context->window, &_x, &_y);

	*x = _x;
	*y = _y;
}

static bool glfw_get_key(hz_context *super, char key)
{
	hz_glfw_context *context = HZ_GLFW_CONTEXT(super);
	return glfwGetKey(context->window, toupper((unsigned char)key)) == GLFW_PRESS;
}

static bool glfw_init(hz_context *super)
{
	hz_glfw_context *context = HZ_GLFW_CONTEXT(super);

	hz_log_i(tag, "running with GLFW %s", glfwGetVersionString());

	if (glfw_init_refcounter == 0 && !glfwInit()) {
		hz_log_e(tag, "unable to init glfw!");
		return false;
	}

	glfw_init_refcounter++;

	context->window = create_context_window();
	if (!context->window) {
		return false;
	}

	glfwSetWindowUserPointer(context->window, super);
	glfwSetFramebufferSizeCallback(context->window, glfw_resize_callback);

	return true;
}

void hz_glfw_context_set_window_title(hz_glfw_context *context, const char *title)
{
	glfwSetWindowTitle(context->window, title);
}

hz_glfw_context *HZ_GLFW_CONTEXT(hz_context *super)
{
	return hz_container_of(super, hz_glfw_context, super);
}

static const hz_context_proto hz_glfw_context_proto = {
	.exit = glfw_exit,
	.poll = glfw_poll,
	.get_key = glfw_get_key,
	.get_mouse_button = glfw_get_mouse_button,
	.get_cursor = glfw_get_cursor,

	.arena_proto = {
		.name = "glfw_context",
		.size = sizeof(hz_glfw_context),
	}
};

hz_context *hz_glfw_context_new(hz_arena *arena)
{
	hz_context *super = hz_context_new(arena, hz_glfw_context, &hz_glfw_context_proto);
	if (!super) {
		return NULL;
	}

	if (!glfw_init(super)) {
		/* FIXME - we don't support freeing pointers from arena yet */
		hz_log_e(tag, "glfw init failed - leaking context");
		return NULL;
	}

	return super;
}
