#include HZ_GL_EXTENSIONS_HEADER
#include <hz/camera.h>
#include <vendor/linmath/linmath.h>
#include <hz/units.h>
#include <internal/helpers/binders.h>

#define CAMERA_DEFAULT_FOV	HZ_DEG_TO_RAD(70.0f)
#define CAMERA_DEFAULT_NEAR	0.1f
#define CAMERA_DEFAULT_FAR	1000.0f
#define CAMERA_DEFAULT_LOOK	(vec3) { 1.0f, 1.0f, 1.0f }
#define CAMERA_DEFAULT_POSITION	(vec3) { 0.0f, 0.0f, 0.0f }

void hz_camera_get_position(struct hz_camera *c, hz_vec3 position)
{
	vec3_dup(position, c->position);
}

static void hz_camera_update_perspective(struct hz_camera *c, size_t w, size_t h)
{
	float aspect;

	aspect = (float)w / h;

	mat4x4_perspective(c->projection, c->fov, aspect, c->near, c->far);
	mat4x4_mul(c->vp, c->projection, c->view);
}

void hz_camera_update_uniforms(struct hz_camera *c)
{
	glUniform3fv(c->uniforms.position, 1, c->position);
}

void hz_camera_update(struct hz_camera *c, size_t w, size_t h)
{
	/* TODO: ortho cameras */
	hz_camera_update_perspective(c, w, h);
}

bool hz_camera_init(struct hz_camera *c, GLuint program)
{
	bool ok;

	struct hz_uniform_binding bindings[] = {
		{ &c->uniforms.position, "camera" },
	};

	ok = hz_bind_uniforms(bindings, program, HZ_ARRAY_SIZE(bindings));
	if (!ok) {
		return false;
	}

	c->fov = CAMERA_DEFAULT_FOV;
	c->near = CAMERA_DEFAULT_NEAR;
	c->far  = CAMERA_DEFAULT_FAR;

	mat4x4_identity(c->view);
	mat4x4_identity(c->vp);

	vec3_dup(c->look, CAMERA_DEFAULT_LOOK);
	vec3_dup(c->position, CAMERA_DEFAULT_POSITION);

	return true;
}
