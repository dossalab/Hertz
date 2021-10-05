#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include "logger.h"

static enum loglevel current_level;

static void format_timestamp(char *buf, size_t size)
{
	time_t rawtime;
	struct tm *timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	snprintf(buf, size, "%02d:%02d:%02d",
		timeinfo->tm_hour,
		timeinfo->tm_min,
		timeinfo->tm_sec
	);
}

static void pretty_print(FILE *stream, const char *label, const char *meta,
		char *fmt, va_list args)
{
	char timestamp[64];

	format_timestamp(timestamp, sizeof(timestamp));

	fprintf(stream, "[%s]\t%s ", label, timestamp);
	if (meta) {
		fprintf(stream, "(%s) ", meta);
	}

	vfprintf(stream, fmt, args);
	fprintf(stream, "\n");
}

void logger_log(const char *label, const char *meta, enum loglevel message_level,
		char *fmt, ...)
{
	va_list args;

	if (message_level >= current_level) {
		va_start(args, fmt);
		pretty_print(stdout, label, meta, fmt, args);
		va_end(args);
	}
}

void logger_init(enum loglevel level)
{
	current_level = level;
}

