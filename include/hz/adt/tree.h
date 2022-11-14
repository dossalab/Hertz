#pragma once

typedef struct _hz_tree_node hz_tree_node;
struct _hz_tree_node {
	hz_tree_node *child, *next, **insert_ptr;
};

typedef void (*hz_tree_traverse_cb)(hz_tree_node *, hz_tree_node *, void *);

void hz_tree_insert(hz_tree_node *parent, hz_tree_node *n);
void hz_tree_traverse(hz_tree_node *parent, hz_tree_node *n,
		hz_tree_traverse_cb cb, void *user);
void hz_tree_traverse_back(hz_tree_node *parent, hz_tree_node *n,
		hz_tree_traverse_cb cb, void *user);

void hz_tree_init(hz_tree_node *n);
