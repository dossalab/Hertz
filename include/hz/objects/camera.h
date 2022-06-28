#ifndef HZ_OBJECTS_CAMERA_H
#define HZ_OBJECTS_CAMERA_H

#include HZ_GL_HEADER
#include <hz/types.h>
#include <stddef.h>

struct hz_camera;
struct hz_object;

void hz_camera_get_vp(struct hz_camera *c, hz_mat4x4 vp);
void hz_camera_resize(struct hz_camera *c, size_t w, size_t h);

struct hz_camera *HZ_CAMERA(struct hz_object *o);
struct hz_object *hz_camera_new(GLuint program);

#endif
