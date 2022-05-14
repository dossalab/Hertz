#ifndef HZ_OBJECTS_ROOT_H
#define HZ_OBJECTS_ROOT_H

#include <hz/object.h>

struct hz_root {
	struct hz_object super;
};

#define hz_cast_root(ptr) \
	hz_container_of(ptr, struct hz_root, super)

void hz_root_init(struct hz_root *r);

#endif
