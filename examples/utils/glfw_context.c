#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <hz/logger.h>
#include <stdbool.h>

#include "glfw_context.h"

#define EXIT_CODE_OK	0
#define EXIT_CODE_ERROR	1

static const char *tag = "glfw";

static void glfw_window_resize_callback(GLFWwindow *window, int width, int height)
{
	struct glfw_ctx_callbacks *callbacks;

	callbacks = glfwGetWindowUserPointer(window);

	if (callbacks->on_resize) {
		callbacks->on_resize(window, width, height, callbacks->user);
	}
}

static GLFWwindow *create_context_window(const char *title)
{
	GLenum glew_ret;
	GLFWwindow *window;

	window = glfwCreateWindow(640, 480, title, NULL, NULL);
	if (!window) {
		hz_log_e(tag, "unable to create window handle!");
		return NULL;
	}

	glfwMakeContextCurrent(window);

	/*
	 * uhh we have to do that *after* context is created.
	 * not sure what will happen if we create multiple windows
	 */
	glew_ret = glewInit();
	if (glew_ret != GLEW_OK) {
		hz_log_e(tag, "unable to init GLEW! %s",
				glewGetErrorString(glew_ret));
		glfwDestroyWindow(window);
		return NULL;
	}

	glfwSetFramebufferSizeCallback(window, glfw_window_resize_callback);

	hz_log_i(tag, "created window with GLEW %s", glewGetString(GLEW_VERSION));
	return window;
}

static bool create_glfw_window_and_draw(const char *title,
		struct glfw_ctx_callbacks *callbacks)
{
	GLFWwindow *window;
	double now, spent, past = glfwGetTime();
	bool ok;

	window = create_context_window(title);
	if (!window) {
		hz_log_e(tag, "unable to create a window!");
		return false;
	}

	glfwSetWindowUserPointer(window, callbacks);

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ok = callbacks->on_init(window, callbacks->user);
	if (!ok) {
		hz_log_e(tag, "'on_init' failed, canceling drawing");
		goto cleanup;
	}

	for (;;) {
		now = glfwGetTime();
		spent = now - past;

		glfwPollEvents();

		if (glfwWindowShouldClose(window)) {
			break;
		}

		callbacks->on_draw(window, spent, callbacks->user);
		glfwSwapBuffers(window);

		past = now;
	}

	callbacks->on_exit(window, callbacks->user);

cleanup:
	glfwDestroyWindow(window);
	return ok;
}

int glfw_ctx_main(const char *title, struct glfw_ctx_callbacks *callbacks)
{
	bool ok;

	hz_log_i(tag, "running with GLFW %s", glfwGetVersionString());

	if (!glfwInit()) {
		hz_log_e(tag, "unable to init glfw!");
		return false;
	}

	ok = create_glfw_window_and_draw(title, callbacks);
	glfwTerminate();

	return ok? EXIT_CODE_OK : EXIT_CODE_ERROR;
}
