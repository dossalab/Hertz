#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#define FILE_IO_CHUNKSIZE	(1024 * 1024)
#define FILE_IO_SLOP		8

static char *mem2str(void *data, size_t written)
{
	char *str = data;

	for (size_t i = 0; i < written; i++) {
		if (!str[i]) {
			str[i] = ' ';
		}
	}

	str[written] = '\0';
	return str;
}

static char *read_text_file_from_stream(FILE *file)
{
	const size_t chunksize = FILE_IO_CHUNKSIZE, slop = FILE_IO_SLOP;

	void *data = NULL, *tmp;
	size_t allocated = 0, written = 0;

	for (;;) {
		size_t nread, space = allocated - written;

		if (space < slop) {
			tmp = realloc(data, slop + allocated + chunksize);
			if (!tmp) {
				break;
			}

			data = tmp;
			allocated += slop + chunksize;
			space += slop + chunksize;
		}

		nread = fread(data + written, 1, space, file);
		if (!nread) {
			break;
		}

		written += nread;
	}

	return mem2str(data, written);
}

char *read_text_file(const char *filename)
{
	char *data;
	FILE *file;

	file = fopen(filename, "r");
	if (!file) {
		return NULL;
	}

	data = read_text_file_from_stream(file);
	fclose(file);

	return data;
}
