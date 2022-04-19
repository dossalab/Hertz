#ifndef HZ_UTILS_CONTAINER_OF_H
#define HZ_UTILS_CONTAINER_OF_H

#include <stddef.h>

#define hz_container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

#endif
