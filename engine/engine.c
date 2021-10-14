#include <engine/engine.h>

static struct camera *current_camera = NULL;
static struct scene *current_scene = NULL;
static mat4x4 projection;

void engine_handle_resize(size_t width, size_t height)
{
	float aspect;

	aspect = (float)width / (float)height;

	mat4x4_perspective(projection, ENG_FOV, aspect, 0.1f, 1000.0f);
	engine_update_mvp();
}

void engine_redraw(void)
{
	scene_redraw(current_scene);
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
