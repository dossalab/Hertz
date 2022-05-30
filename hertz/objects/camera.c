#include HZ_GL_EXTENSIONS_HEADER
#include <hz/objects/camera.h>
#include <vendor/linmath/linmath.h>
#include <internal/alloc.h>
#include <internal/object.h>
#include <stdbool.h>
#include <hz/utils/container_of.h>
#include <internal/helpers/binders.h>
#include <hz/units.h>

struct hz_camera {
	struct hz_object super;
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

void hz_camera_get_vp(struct hz_camera *c, hz_mat4x4 vp)
{
	mat4x4_dup(vp, c->vp);
}

static void hz_camera_resize_perspective(struct hz_camera *c, size_t w, size_t h)
{
	float aspect;

	aspect = (float)w / h;

	mat4x4_perspective(c->projection, c->fov, aspect, c->near, c->far);
	mat4x4_mul(c->vp, c->projection, c->view);
}

void hz_camera_resize(struct hz_camera *c, size_t w, size_t h)
{
	/* TODO: ortho cameras */
	hz_camera_resize_perspective(c, w, h);
}

static void camera_bind(struct hz_object *super)
{
	struct hz_camera *c = HZ_CAMERA(super);
	vec3 position, scale;
	quat rotation;

	/* we obviuosly only care about position */
	mat4x4_decompose(super->model, scale, rotation, position);

	glUniform3fv(c->uniforms.position, 1, position);

	mat4x4_invert(c->view, super->model);
	mat4x4_mul(c->vp, c->projection, c->view);
}

static void camera_draw(struct hz_object *super, struct hz_camera *_)
{
	/* pass */
}

const struct hz_object_proto hz_camera_proto = {
	.bind = camera_bind,
	.draw = camera_draw,
};

static bool camera_init(struct hz_camera *c, GLuint program)
{
	bool ok;
	struct hz_object *super = HZ_OBJECT(c);

	struct hz_uniform_binding bindings[] = {
		{ &c->uniforms.position, "camera" },
	};

	hz_object_init(super, &hz_camera_proto);

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

struct hz_camera *HZ_CAMERA(struct hz_object *o)
{
	return hz_container_of(o, struct hz_camera, super);
}

struct hz_object *hz_camera_new(GLuint program)
{
	return HZ_OBJECT(hz_alloc_and_init(struct hz_camera, camera_init, program));
}
