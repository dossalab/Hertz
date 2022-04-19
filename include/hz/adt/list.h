#ifndef HZ_ADT_LIST_H
#define HZ_ADT_LIST_H

struct hz_list_item {
	struct hz_list_item *next, *prev;
};

#define BZ_LIST_CREATE(name) \
	struct hz_list_item name = { .next = &(name), .prev = &(name) }

#define hz_list_forward(ptr, list) \
	for (ptr = (list)->next; ptr != (list); ptr = ptr->next)

#define hz_list_backward(ptr, list) \
	for (ptr = (list)->prev; ptr != (list); ptr = ptr->prev)

#define hz_list_forward_safe(ptr, n, list) \
	for (ptr = (list)->next, n = ptr->next; ptr != (list); \
		ptr = n, n = ptr->next)

#define hz_list_backward_safe(ptr, n, list) \
	for (ptr = (list)->prev, n = ptr->prev; ptr != (list); \
		ptr = n, n = ptr->prev)

void hz_list_init(struct hz_list_item *head);

void hz_list_insert(struct hz_list_item *new,
			struct hz_list_item *prev,
			struct hz_list_item *next);

void hz_list_push(struct hz_list_item *new, struct hz_list_item *head);

#endif
