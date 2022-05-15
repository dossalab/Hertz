#ifndef HZ_UTILS_COMPILER_H
#define HZ_UTILS_COMPILER_H

#define hz_likely(x)	__builtin_expect(!!(x), 1)
#define hz_unlikely(x)	__builtin_expect(!!(x), 0)

#endif
