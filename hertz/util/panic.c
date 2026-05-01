#include <hz/util/panic.h>
#include <stdio.h>
#include <stdlib.h>

void hz_panic(const char *msg)
{
	fprintf(stderr, "PANIC: %s\n", msg);
	abort();
}
