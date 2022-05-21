#ifndef HZ_OBJECTS_ROOT_H
#define HZ_OBJECTS_ROOT_H

struct hz_root;
struct hz_object;

struct hz_root *HZ_ROOT(struct hz_object *o);
struct hz_object *hz_root_new(void);

#endif
