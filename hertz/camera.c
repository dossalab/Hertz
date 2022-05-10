#include <hz/camera.h>
#include <vendor/linmath/linmath.h>
#include <hz/helpers/binders.h>

void hz_camera_update(struct hz_camera *c, size_t width, size_t height)
{
	c->proto->update(c, width, height);
}

void hz_camera_get_position(struct hz_camera *c, hz_vec3 position)
{
	vec3_dup(position, c->position);
}

bool hz_camera_init(struct hz_camera *c, const struct hz_camera_proto *proto,
		GLuint program)
{
	bool ok;

	struct hz_uniform_binding bindings[] = {
		{ &c->uniforms.position, "camera" },
	};

	ok = hz_bind_uniforms(bindings, program, HZ_ARRAY_SIZE(bindings));
	if (!ok) {
		return false;
	}

	c->proto = proto;

	mat4x4_identity(c->view);
	mat4x4_identity(c->vp);

	c->proto->probe(c);

	return true;
}
