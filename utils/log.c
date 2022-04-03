#include <stdio.h>
#include <stdarg.h>
#include "log.h"

void _log(const char *expl, const char *fmt, ...)
{
	va_list args;
	char line[LOGLINE_MAXLEN];

	va_start(args, fmt);
	vsnprintf(line, LOGLINE_MAXLEN, fmt, args);
	printf("%s%s\n", expl, line);
	va_end(args);
}
