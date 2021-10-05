#include "list.h"

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
