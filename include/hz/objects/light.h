#ifndef HZ_OBJECTS_LIGHT_H
#define HZ_OBJECTS_LIGHT_H

#include HZ_GL_HEADER

struct hz_light;
struct hz_object;

struct hz_light *HZ_LIGHT(struct hz_object *o);

void hz_light_setup(struct hz_light *l, float kc, float kl, float kq);
void hz_light_dim(struct hz_light *l, float intensity);

struct hz_object *hz_light_new(GLuint program, unsigned index);

#endif
