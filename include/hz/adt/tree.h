#ifndef HZ_ADT_TREE_H
#define HZ_ADT_TREE_H

struct hz_tree_node {
	struct hz_tree_node *child, *next, **insert_ptr;
};

typedef void (*hz_tree_traverse_cb)(struct hz_tree_node *,
		struct hz_tree_node *, void *);

void hz_tree_insert(struct hz_tree_node *parent, struct hz_tree_node *n);
void hz_tree_traverse(struct hz_tree_node *parent, struct hz_tree_node *n,
		hz_tree_traverse_cb cb, void *user);
void hz_tree_traverse_back(struct hz_tree_node *parent, struct hz_tree_node *n,
		hz_tree_traverse_cb cb, void *user);
void hz_tree_init(struct hz_tree_node *n);

#endif
