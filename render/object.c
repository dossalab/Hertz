#include <utils/glfw_context.h>
#include "object.h"

void object_draw(struct object *o)
{
	glBindVertexArray(o->vao);
	o->proto->draw(o);
}

void object_update_mvp(struct object *o, mat4x4 vp)
{
	o->proto->update_mvp(o, vp);
}

void object_free(struct object *o)
{
	o->proto->free(o);
	glDeleteVertexArrays(1, &o->vao);
}

void object_init(struct object *o, const struct object_proto *proto)
{
	glGenVertexArrays(1, &o->vao);
	glBindVertexArray(o->vao);

	o->proto = proto;
}
