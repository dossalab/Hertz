#ifndef HZ_LOGGER_H
#define HZ_LOGGER_H

#define HZ_LOG_EXPAND(x)	#x
#define HZ_LOG_NUM_TO_STR(x)	HZ_LOG_EXPAND(x)
#define HZ_LOG_FILE_AND_LINE	__FILE__ ":" HZ_LOG_NUM_TO_STR(__LINE__)

#define HZ_LOG_MAKE_COLOR(color, label) \
	color label "\e[0m"

#define HZ_LOG_RED(x)	HZ_LOG_MAKE_COLOR("\e[0;31m", x)
#define HZ_LOG_GRN(x)	HZ_LOG_MAKE_COLOR("\e[0;32m", x)
#define HZ_LOG_YEL(x)	HZ_LOG_MAKE_COLOR("\e[0;33m", x)
#define HZ_LOG_BLU(x)	HZ_LOG_MAKE_COLOR("\e[0;34m", x)
#define HZ_LOG_MAG(x)	HZ_LOG_MAKE_COLOR("\e[0;35m", x)
#define HZ_LOG_CYA(x)	HZ_LOG_MAKE_COLOR("\e[0;36m", x)

enum hz_loglevel {
	HZ_LOGLEVEL_INFO,
	HZ_LOGLEVEL_ERROR,
	HZ_LOGLEVEL_SILENT
};

typedef int (*hz_logger_sink)(const char *, ...);

extern hz_logger_sink *hz_logger_info_sink;
extern hz_logger_sink *hz_logger_error_sink;

#define hz_log_fmt(sink, tag, expl, fmt, ...) \
	(*sink)("%6s " expl " " fmt "\n", tag, ## __VA_ARGS__)

#define hz_log_i(tag, ...) \
	hz_log_fmt(hz_logger_info_sink, tag, HZ_LOG_GRN("[I]"), __VA_ARGS__)

#define hz_log_e(tag, ...) \
	hz_log_fmt(hz_logger_error_sink, tag, HZ_LOG_RED("[E] (" HZ_LOG_FILE_AND_LINE ")"),\
			__VA_ARGS__)

void hz_logger_init(enum hz_loglevel level);
void hz_logger_route_output(hz_logger_sink to);

#endif
