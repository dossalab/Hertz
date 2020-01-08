#include <engine/scene.h>
#include <engine/camera.h>

void camera_init(struct camera *c)
{
	mat4x4_identity(c->view);
}

