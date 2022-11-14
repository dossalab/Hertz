#include <hz/adt/tree.h>
#include <stddef.h>

void hz_tree_insert(hz_tree_node *parent, hz_tree_node *n)
{
	*parent->insert_ptr = n;
	parent->insert_ptr = &n->next;
}

void hz_tree_traverse(hz_tree_node *parent, hz_tree_node *n,
		hz_tree_traverse_cb cb, void *user)
{
	hz_tree_node *ptr;

	cb(parent, n, user);

	for (ptr = n->child; ptr; ptr = ptr->next) {
		hz_tree_traverse(n, ptr, cb, user);
	}
}

void hz_tree_traverse_back(hz_tree_node *parent, hz_tree_node *n,
		hz_tree_traverse_cb cb, void *user)
{
	hz_tree_node *ptr;

	for (ptr = n->child; ptr; ptr = ptr->next) {
		hz_tree_traverse_back(n, ptr, cb, user);
	}

	cb(parent, n, user);
}

void hz_tree_init(hz_tree_node *n)
{
	n->child = NULL;
	n->next = NULL;
	n->insert_ptr = &n->child;
}
