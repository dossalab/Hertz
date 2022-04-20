#include GL_EXTENSIONS_HEADER
#include <hz/object.h>
#include <vendor/linmath/linmath.h>

void hz_object_draw(struct hz_object *o)
{
	glBindVertexArray(o->vao);
	glUseProgram(o->program);
	o->proto->draw(o);
}

void hz_object_update_mvp(struct hz_object *o, mat4x4 vp)
{
	o->proto->update_mvp(o, vp);
}

void hz_object_deinit(struct hz_object *o)
{
	o->proto->deinit(o);
	glDeleteVertexArrays(1, &o->vao);
}

bool hz_object_init(struct hz_object *o, GLuint program,
		const struct hz_object_proto *proto)
{
	o->proto = proto;
	o->program = program;

	glGenVertexArrays(1, &o->vao);
	glBindVertexArray(o->vao);
	glUseProgram(o->program);

	return o->proto->init(o);
}
