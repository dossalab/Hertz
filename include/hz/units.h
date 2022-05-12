#ifndef HZ_UNITS_H
#define HZ_UNITS_H

#include <math.h>

#ifdef M_PI
 #define HZ_M_PI	M_PI
#else
 #define HZ_M_PI	3.14159265358979323846
#endif

#define HZ_DEG_TO_RAD(x)	((x) * HZ_M_PI / 180.0f)
#define HZ_RAD_TO_DEG(x)	((x) / HZ_M_PI * 180.f)

#endif
