#include HZ_GL_EXTENSIONS_HEADER
#include <hz/nodes/camera.h>
#include <vendor/linmath/linmath.h>
#include <hz/internal/node.h>
#include <stdbool.h>
#include <hz/internal/helpers/binders.h>
#include <hz/units.h>

struct _hz_camera {
	hz_node super;
	hz_mat4x4 view, projection, vp;
	float fov, near, far;

	struct {
		GLint position;
	} uniforms;
};

#define CAMERA_DEFAULT_FOV	HZ_DEG_TO_RAD(70.0f)
#define CAMERA_DEFAULT_NEAR	0.1f
#define CAMERA_DEFAULT_FAR	1000.0f
#define CAMERA_DEFAULT_LOOK	(vec3) { 1.0f, 1.0f, 1.0f }

void hz_camera_get_vp(hz_camera *c, hz_mat4x4 vp)
{
	mat4x4_dup(vp, c->vp);
}

static void hz_camera_resize_perspective(hz_camera *c, size_t w, size_t h)
{
	float aspect;

	aspect = (float)w / h;

	mat4x4_perspective(c->projection, c->fov, aspect, c->near, c->far);
	mat4x4_mul(c->vp, c->projection, c->view);
}

void hz_camera_resize(hz_camera *c, unsigned w, unsigned h)
{
	/* TODO: ortho cameras */
	hz_camera_resize_perspective(c, w, h);
}

static void camera_bind(hz_node *super)
{
	hz_camera *c = HZ_CAMERA(super);
	vec3 position, scale;
	quat rotation;

	/* we obviuosly only care about position */
	mat4x4_decompose(super->model, scale, rotation, position);

	glUniform3fv(c->uniforms.position, 1, position);

	mat4x4_invert(c->view, super->model);
	mat4x4_mul(c->vp, c->projection, c->view);
}

hz_camera *HZ_CAMERA(hz_node *o)
{
	return hz_container_of(o, hz_camera, super);
}

static bool camera_init(hz_node *super, GLuint program)
{
	bool ok;
	hz_camera *c = HZ_CAMERA(super);

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

	return true;
}

const hz_node_proto hz_camera_proto = {
	.bind = camera_bind,
	.draw = hz_node_dummy_draw,
	.arena_proto = {
		.name = "cameras",
		.size = sizeof(hz_camera)
	}
};

hz_node *hz_camera_new(hz_arena *arena, GLuint program)
{
	hz_node *super = hz_node_new(arena, hz_camera, &hz_camera_proto);
	if (!super) {
		return NULL;
	}

	camera_init(super, program);

	return super;
}
