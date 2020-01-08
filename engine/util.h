#ifndef ENGINE_UTILS_H
#define ENGINE_UTILS_H

#include <stddef.h>

struct list_item {
	struct list_item *next, *prev;
};

#define LIST_CREATE(name) \
	struct list_item name = { .next = &(name), .prev = &(name) }

#define list_forward(ptr, list) \
	for (ptr = (list)->next; ptr != (list); ptr = ptr->next)

#define list_backward(ptr, list) \
	for (ptr = (list)->prev; ptr != (list); ptr = ptr->prev)

#define list_forward_safe(ptr, n, list) \
	for (ptr = (list)->next, n = ptr->next; ptr != (list); \
		ptr = n, n = ptr->next)

#define list_backward_safe(ptr, n, list) \
	for (ptr = (list)->prev, n = ptr->prev; ptr != (list); \
		ptr = n, n = ptr->prev)

void list_init(struct list_item *head);

void list_insert(struct list_item *new,
			struct list_item *prev,
			struct list_item *next);

void list_push(struct list_item *new, struct list_item *head);

#define streq(x, y)	(strcmp((x), (y)) == 0)
#define container_of(ptr, type, member) ({                      \
	const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
	(type *)( (char *)__mptr - offsetof(type,member) );})

#define ARRAY_LEN(X)	(sizeof(X) / sizeof(X[0]))

struct region {
	void *ptr;
	unsigned int size;
	unsigned int alloc;
};

int read_file(int fd, struct region *region);
int read_path(const char *path, struct region *region);
void logger_report(char *fmt, ...);

#endif

