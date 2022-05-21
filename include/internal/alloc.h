#ifndef HZ_INTERNAL_ALLOC_H
#define HZ_INTERNAL_ALLOC_H

#include <stdlib.h> /* for *alloc / free */

#define hz_alloc_and_init(type, init, ...) ({ \
	type *o = calloc(1, sizeof(type)); \
	if (!o) { \
		return NULL; \
	} \
	if (!init(o, ## __VA_ARGS__)) { \
		free(o); \
		return NULL; \
	} \
	o; \
})

#endif
