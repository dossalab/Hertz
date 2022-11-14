#pragma once

#include <stddef.h>

#define HZ_ARRAY_SIZE(x)	(sizeof(x) / sizeof(x[0]))
#define hz_container_of(ptr, type, member) ({ \
		const typeof(((type *)0)->member ) *__mptr = (ptr); \
		(type *)((char *)__mptr - offsetof(type, member)); \
	})
