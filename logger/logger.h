#ifndef LOGGER_LOGGER_H
#define LOGGER_LOGGER_H

#include <stddef.h> /* for NULL */

enum loglevel {
	LOGLEVEL_DEBUG,
	LOGLEVEL_INFO,
	LOGLEVEL_WARN,
	LOGLEVEL_ERROR,
	LOGLEVEL_SILENT, /* Should be the last (highest) one */
};

#define log_d(...) logger_log("DEBUG", NULL, LOGLEVEL_DEBUG, __VA_ARGS__)
#define log_i(...) logger_log("INFO", NULL, LOGLEVEL_INFO, __VA_ARGS__)
#define log_w(...) logger_log("WARN", NULL, LOGLEVEL_WARN, __VA_ARGS__)
#define log_e(...) logger_log("ERROR", __FILE__, LOGLEVEL_ERROR, __VA_ARGS__)

void logger_log(const char *label, const char *meta, enum loglevel level,
		char *fmt, ...);
void logger_init(enum loglevel level);

#endif
