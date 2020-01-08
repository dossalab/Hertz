#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <engine/util.h>

#define CHUNKSIZE	(1024 * 8)

void list_init(struct list_item *head)
{
	head->next = head;
	head->prev = head;
}

void list_insert(struct list_item *new,
			struct list_item *prev,
			struct list_item *next)
{
	new->next = next;
	next->prev = new;
	prev->next = new;
	new->prev = prev;
}

void list_push(struct list_item *new, struct list_item *head)
{
	list_insert(new, head, head->next);
}

int read_file(int fd, struct region *region)
{
	int len = 0, done = 0;
	char *buf = NULL;

	for (;;) {
		int space = len - done, ret;
		if (!space) {
			space = CHUNKSIZE;
			len += space;
			buf = realloc(buf, len);
		}
		ret = read(fd, buf + done, space);
		if (ret > 0) {
			done += ret;
			continue;
		}
		if (!ret)
			break;
		if (errno == EINTR || errno == EAGAIN)
			continue;
		free(buf);
		return -1;
	}

	if (!done) {
		free(buf);
		return -1;
	}

	/* "len+8" guarantees that there is some slop at the end */
	region->ptr = realloc(buf, len+8);
	region->size = done;
	region->alloc = len;
	return 0;
}

int read_path(const char *path, struct region *region)
{
	int fd = open(path, O_RDONLY);
	if (fd > 0) {
		int ret = read_file(fd, region);
		int saved_errno = errno;
		close(fd);
		errno = saved_errno;
		return ret;
	}
	return fd;
}

void logger_report(char *fmt, ...)
{
	va_list params;

	va_start(params, fmt);
	vprintf(fmt, params);
	va_end(params);
}
