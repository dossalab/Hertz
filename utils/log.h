#ifndef UTILS_LOG_H
#define UTILS_LOG_H

#define LOGGER_EXPAND(x)	#x
#define LOGGER_NUM_TO_STR(x)	LOGGER_EXPAND(x)
#define LOGGER_FILE_AND_LINE	__FILE__ ":" LOGGER_NUM_TO_STR(__LINE__)

#define LOG_MAKE_COLOR(color, label) \
	color label "\e[0m"

#define LOG_RED(x)	LOG_MAKE_COLOR("\e[0;31m", x)
#define LOG_GRN(x)	LOG_MAKE_COLOR("\e[0;32m", x)
#define LOG_YEL(x)	LOG_MAKE_COLOR("\e[0;33m", x)
#define LOG_BLU(x)	LOG_MAKE_COLOR("\e[0;34m", x)
#define LOG_MAG(x)	LOG_MAKE_COLOR("\e[0;35m", x)
#define LOG_CYA(x)	LOG_MAKE_COLOR("\e[0;36m", x)

enum loglevel {
	LOGLEVEL_INFO,
	LOGLEVEL_ERROR,
	LOGLEVEL_SILENT
};

typedef int (*logger)(const char *, ...);

extern logger do_log_i;
extern logger do_log_e;

#define _log(fun, tag, expl, fmt, ...) \
	fun("%6s " expl " " fmt "\n", tag __VA_OPT__(,) __VA_ARGS__)

#define log_i(tag, ...) \
	_log(do_log_i, tag, LOG_GRN("[I]"), __VA_ARGS__)

#define log_e(tag, ...) \
	_log(do_log_e, tag, LOG_RED("[E] (" LOGGER_FILE_AND_LINE ")"), __VA_ARGS__)

void logger_init(enum loglevel level);

#endif
