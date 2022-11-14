#include <hz/adt/list.h>

void hz_list_init(hz_list_item *head)
{
	head->next = head;
	head->prev = head;
}

void hz_list_insert(hz_list_item *new, hz_list_item *prev, hz_list_item *next)
{
	new->next = next;
	next->prev = new;
	prev->next = new;
	new->prev = prev;
}

void hz_list_push(hz_list_item *new, hz_list_item *head)
{
	hz_list_insert(new, head, head->next);
}
