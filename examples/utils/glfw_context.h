#ifndef EXAMPLE_GLFW_CONTEXT_H
#define EXAMPLE_GLFW_CONTEXT_H

#include <stddef.h>
#include <stdbool.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef bool (*glfw_init_cb)(GLFWwindow *window, void *user);
typedef void (*glfw_draw_cb)(GLFWwindow *window, double time_spent, void *user);
typedef void (*glfw_resize_cb)(GLFWwindow *window, size_t w, size_t h, void *user);
typedef void (*glfw_exit_cb)(GLFWwindow *window, void *user);

struct glfw_ctx_callbacks {
	glfw_init_cb on_init;
	glfw_draw_cb on_draw;
	glfw_resize_cb on_resize;
	glfw_exit_cb on_exit;

	void *user;
};

int glfw_ctx_main(const char *title, struct glfw_ctx_callbacks *callbacks);

#endif
