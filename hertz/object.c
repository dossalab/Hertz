#include <hz/object.h>
#include <hz/camera.h>

void hz_object_draw(struct hz_object *o, struct hz_camera *c)
{
	o->proto->draw(o, c);
}

void hz_object_deinit(struct hz_object *o)
{
	o->proto->deinit(o);
}

void hz_object_set_proto(struct hz_object *o, const struct hz_object_proto *proto)
{
	o->proto = proto;
}
