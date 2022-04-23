#include <hz/camera.h>
#include <vendor/linmath/linmath.h>

void hz_camera_update(struct hz_camera *c, size_t width, size_t height)
{
	c->proto->update(c, width, height);
}

void hz_camera_init(struct hz_camera *c, const struct hz_camera_proto *proto)
{
	c->proto = proto;

	mat4x4_identity(c->view);
	mat4x4_identity(c->vp);

	c->proto->probe(c);
}
