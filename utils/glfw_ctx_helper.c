#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <utils/log.h>
#include <stdbool.h>

#include "glfw_ctx_helper.h"

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
		log_e("unable to create window handle!");
		return NULL;
	}

	glfwMakeContextCurrent(window);

	/*
	 * uhh we have to do that *after* context is created.
	 * not sure what will happen if we create multiple windows
	 */
	glew_ret = glewInit();
	if (glew_ret != GLEW_OK) {
		log_e("unable to init GLEW! %s", glewGetErrorString(glew_ret));
		glfwDestroyWindow(window);
		return NULL;
	}

	glfwSetFramebufferSizeCallback(window, glfw_window_resize_callback);

	log_i("created window with GLEW %s", glewGetString(GLEW_VERSION));
	return window;
}

static bool create_glfw_window_and_draw(const char *title,
		struct glfw_ctx_callbacks *callbacks)
{
	GLFWwindow *window;
	double now, spent, past;
	bool ok;

	window = create_context_window(title);
	if (!window) {
		log_e("unable to create a window!");
		return false;
	}

	glfwSetWindowUserPointer(window, callbacks);

	ok = callbacks->on_init(window, callbacks->user);
	if (!ok) {
		log_e("'on_init' failed, canceling drawing");
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

bool glfw_ctx_main(const char *title, struct glfw_ctx_callbacks *callbacks)
{
	bool ok;

	if (!glfwInit()) {
		log_e("unable to init glfw!");
		return false;
	}

	ok = create_glfw_window_and_draw(title, callbacks);
	glfwTerminate();

	return ok;
}
