#ifndef UTILS_LOG_H
#define UTILS_LOG_H

#include <stdio.h>

#define LOGLINE_MAXLEN		120

#define logger_expand(x)       #x
#define logger_line_to_str(x)  logger_expand(x)
#define logger_file_and_line   __FILE__":"logger_line_to_str(__LINE__)

#define LOG_FORMAT(color, label) \
	color label LOG_COLOR_CLEAR

#define LOG_COLOR_RED(x)	LOG_FORMAT("\e[0;31m", x)
#define LOG_COLOR_GRN(x)	LOG_FORMAT("\e[0;32m", x)
#define LOG_COLOR_YEL(x)	LOG_FORMAT("\e[0;33m", x)
#define LOG_COLOR_BLU(x)	LOG_FORMAT("\e[0;34m", x)
#define LOG_COLOR_MAG(x)	LOG_FORMAT("\e[0;35m", x)
#define LOG_COLOR_CYA(x)	LOG_FORMAT("\e[0;36m", x)
#define LOG_COLOR_CLEAR		"\e[0m"

void _log(const char *expl, const char *fmt, ...);

#define log_i(...) _log(LOG_COLOR_GRN("[INFO]") "\t", __VA_ARGS__)
#define log_e(...) _log(LOG_COLOR_RED("[ERROR]") "\t(" logger_file_and_line ") ", \
		__VA_ARGS__)

#endif
