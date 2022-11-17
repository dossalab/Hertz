#pragma once

#include HZ_GL_HEADER
#include <hz/types.h>
#include <hz/node.h>

typedef struct _hz_camera hz_camera;

void hz_camera_get_vp(hz_camera *c, hz_mat4x4 vp);
void hz_camera_resize(hz_camera *c, unsigned w, unsigned h);

hz_camera *HZ_CAMERA(hz_node *o);
hz_node *hz_camera_new(hz_arena *arena, GLuint program);
