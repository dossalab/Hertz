#ifndef HZ_CAMERA_H
#define HZ_CAMERA_H

#include <hz/types.h>
#include <stddef.h>
#include <stdbool.h>
#include <GL/gl.h>

struct hz_camera;

struct hz_camera_proto {
	void (*probe)(struct hz_camera *);
	void (*update)(struct hz_camera *, size_t width, size_t height);
};

struct hz_camera {
	hz_mat4x4 view, projection, vp;
	hz_vec3 position;

	struct {
		GLint position;
	} uniforms;

	const struct hz_camera_proto *proto;
};

#define hz_cast_camera(ptr) &(ptr)->super

void hz_camera_get_position(struct hz_camera *c, hz_vec3 position);
void hz_camera_update(struct hz_camera *c, size_t width, size_t height);
bool hz_camera_init(struct hz_camera *c, const struct hz_camera_proto *proto,
		GLuint program);

#endif
