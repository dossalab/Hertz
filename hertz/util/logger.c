#include <stdio.h>
#include <hz/util/logger.h>

static int discard(const char *fmt, ...) {
	return 0;
}

static hz_logger_sink active_sink = printf;
static hz_logger_sink discard_sink = discard;

hz_logger_sink *hz_logger_info_sink = &discard_sink;
hz_logger_sink *hz_logger_error_sink = &discard_sink;

static void assign_sink(hz_logger_sink **s, hz_loglevel level, hz_loglevel setting)
{
	*s = (setting > level)? &discard_sink : &active_sink;
}

void hz_logger_init(hz_loglevel level)
{
	assign_sink(&hz_logger_info_sink, HZ_LOGLEVEL_INFO, level);
	assign_sink(&hz_logger_error_sink, HZ_LOGLEVEL_ERROR, level);
}

void hz_logger_route_output(hz_logger_sink to)
{
	active_sink = to;
}
