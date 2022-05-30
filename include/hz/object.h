#ifndef HZ_OBJECT_H
#define HZ_OBJECT_H

#include <stdbool.h>
#include <hz/types.h>

struct hz_object;
struct hz_camera;

void hz_object_insert(struct hz_object *o, struct hz_object *child);
void hz_object_move(struct hz_object *o, hz_vec3 pos);
void hz_object_rotate(struct hz_object *o, float angle, hz_vec3 axis);
void hz_object_rotate_quat(struct hz_object *o, hz_quat q);
void hz_object_scale(struct hz_object *o, hz_vec3 scale);
void hz_object_set_model(struct hz_object *o, hz_mat4x4 model, bool transpose);
void hz_object_draw(struct hz_object *o, struct hz_camera *c);
void hz_object_update(struct hz_object *o);

#endif
