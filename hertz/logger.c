#include <stdio.h>
#include <hz/logger.h>

static int sink(const char *fmt, ...) {
	return 0;
}

hz_logger hz_do_log_i = sink;
hz_logger hz_do_log_e = sink;

static void assign_logger(hz_logger *l, enum hz_loglevel level,
		enum hz_loglevel setting)
{
	*l = (setting > level)? sink : printf;
}

void hz_logger_init(enum hz_loglevel level)
{
	assign_logger(&hz_do_log_i, HZ_LOGLEVEL_INFO, level);
	assign_logger(&hz_do_log_e, HZ_LOGLEVEL_ERROR, level);
}
