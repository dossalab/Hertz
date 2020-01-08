#include <engine/engine.h>
#include <engine/events.h>

static GLFWwindow *main_window = NULL;
static struct scene *current_scene = NULL;
static struct camera *current_camera = NULL;
static mat4x4 projection;

static void error_callback(int error, const char* description)
{
	logger_report("[GLFW] %s!\n", description);
}

static void update_projection(int width, int height)
{
	float aspect;

	glViewport(0, 0, width, height);
	aspect = (float)width / (float)height;

	mat4x4_perspective(projection, ENG_FOV, aspect, 0.1f, 1000.0f);
}

static void resize_callback(GLFWwindow* window, int width, int height)
{
	update_projection(width, height);
	engine_update_mvp();
}

int engine_init(void)
{
	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) {
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	main_window = glfwCreateWindow(640, 480, "test", NULL, NULL);
	if (!main_window) {
		return -1;
	}

	update_projection(640, 480);

	glfwMakeContextCurrent(main_window);
	glfwSetFramebufferSizeCallback(main_window, resize_callback);
	//glewInit();

	//glfwWindowHint(GLFW_SAMPLES, 4);
	//glEnable(GL_MULTISAMPLE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	engine_events_init();

	return 0;
}

int engine_poll(void)
{
	glfwPollEvents();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene_redraw(current_scene);
	glfwSwapBuffers(main_window);

	return -glfwWindowShouldClose(main_window);
}

void engine_update_mvp(void)
{
	mat4x4 vp;

	mat4x4_mul(vp, projection, current_camera->view);
	scene_update_mvp(current_scene, vp);
}

void engine_set_scene(struct scene *s)
{
	current_scene = s;
}

void engine_set_camera(struct camera *c)
{
	current_camera = c;
}

GLFWwindow *engine_get_window(void)
{
	return main_window;
}

void engine_exit(void)
{
	glfwDestroyWindow(main_window);
	glfwTerminate();
}

