#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <logger/logger.h>
#include <errors/errors.h>

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

static int create_glfw_window_and_draw(const char *title,
		struct glfw_ctx_callbacks *callbacks)
{
	GLFWwindow *window;
	double now, spent, past;
	int err = 0;

	window = create_context_window(title);
	if (!window) {
		log_e("unable to create a window!");
		return -ERR_SYSTEM;
	}

	glfwSetWindowUserPointer(window, callbacks);

	err = callbacks->on_init(window, callbacks->user);
	if (err < 0) {
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
	return err;
}

int glfw_ctx_main(const char *title, struct glfw_ctx_callbacks *callbacks)
{
	int err;

	if (!glfwInit()) {
		log_e("unable to init glfw!");
		return -ERR_SYSTEM;
	}

	err = create_glfw_window_and_draw(title, callbacks);
	glfwTerminate();

	return err;
}
