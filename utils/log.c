#include <stdio.h>
#include "log.h"

static int sink(const char *fmt, ...) {
	return 0;
}

logger do_log_i = sink;
logger do_log_e = sink;

static void assign_logger(logger *l, enum loglevel level, enum loglevel setting) {
	*l = (setting > level)? sink : printf;
}

void logger_init(enum loglevel level)
{
	assign_logger(&do_log_i, LOGLEVEL_INFO, level);
	assign_logger(&do_log_e, LOGLEVEL_ERROR, level);
}
