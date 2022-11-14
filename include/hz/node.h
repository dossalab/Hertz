#pragma once

#include <hz/types.h>
#include <stdbool.h>

typedef struct _hz_node hz_node;
typedef struct _hz_camera hz_camera;

void hz_node_insert(hz_node *n, hz_node *child);
void hz_node_move(hz_node *n, hz_vec3 pos);
void hz_node_rotate(hz_node *n, float angle, hz_vec3 axis);
void hz_node_rotate_quat(hz_node *n, hz_quat q);
void hz_node_scale(hz_node *n, hz_vec3 scale);
void hz_node_set_model(hz_node *n, hz_mat4x4 model, bool transpose);
void hz_node_draw(hz_node *n, hz_camera *c);
void hz_node_update(hz_node *n);
