#pragma once

#include <stddef.h>
#include <stdlib.h>
#include <hz/util/panic.h>

static inline void *hz_malloc(size_t size)
{
	void *ptr = malloc(size);
	if (!ptr)
		hz_panic("out of memory");
	return ptr;
}

static inline void hz_free(void *ptr)
{
	free(ptr);
}
