#include GL_EXTENSIONS_HEADER
#include "object.h"

void object_draw(struct object *o)
{
	glBindVertexArray(o->vao);
	glUseProgram(o->program);
	o->proto->draw(o);
}

void object_update_mvp(struct object *o, mat4x4 vp)
{
	o->proto->update_mvp(o, vp);
}

void object_deinit(struct object *o)
{
	o->proto->deinit(o);
	glDeleteVertexArrays(1, &o->vao);
}

bool object_init(struct object *o, GLuint program, const struct object_proto *proto)
{
	o->proto = proto;
	o->program = program;

	glGenVertexArrays(1, &o->vao);
	glBindVertexArray(o->vao);
	glUseProgram(o->program);

	return o->proto->init(o);
}
