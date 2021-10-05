#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

static char *terminate_buffer(char *buffer, size_t len, size_t boundary)
{
	char *new_buffer;
	const size_t slop = 8;

	if (boundary >= len) {
		new_buffer = realloc(buffer, len + slop);
		if (!new_buffer) {
			/* ah, come on! */
			free(buffer);
			return NULL;
		}

		buffer = new_buffer;
	}

	buffer[boundary] = '\0';
	return buffer;
}

static char *read_all_from_stream(FILE *file)
{
	const size_t chunksize = 1024;

	char *buffer = NULL, *new_buffer;
	size_t buffer_len = 0;
	size_t written = 0;

	for (;;) {
		size_t free_space = buffer_len - written;
		size_t amount_read;

		if (!free_space) {
			new_buffer = realloc(buffer, buffer_len + chunksize);
			if (!new_buffer) {
				break;
			}

			buffer = new_buffer;
			buffer_len += chunksize;
			free_space += chunksize;
		}

		amount_read = fread(buffer + written, 1, free_space, file);
		if (amount_read <= 0) {
			break;
		}

		written += amount_read;
	}

	return terminate_buffer(buffer, buffer_len, written);
}

char *read_text_file(const char *filename)
{
	char *data;
	FILE *file;

	file = fopen(filename, "r");
	if (!file) {
		return NULL;
	}

	data = read_all_from_stream(file);
	fclose(file);

	return data;
}
