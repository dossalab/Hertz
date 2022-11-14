#pragma once

typedef struct _hz_list_item hz_list_item;
struct _hz_list_item {
	hz_list_item *next, *prev;
};

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


void hz_list_insert(hz_list_item *new, hz_list_item *prev, hz_list_item *next);
void hz_list_push(hz_list_item *new, hz_list_item *head);

void hz_list_init(hz_list_item *head);
