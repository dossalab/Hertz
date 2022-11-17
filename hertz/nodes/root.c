#include <hz/nodes/root.h>
#include <hz/internal/node.h>

struct _hz_root {
	hz_node super;
};

hz_root *HZ_ROOT(hz_node *n)
{
	return hz_container_of(n, hz_root, super);
}

static const hz_node_proto hz_root_proto = {
	.bind = hz_node_dummy_bind,
	.draw = hz_node_dummy_draw,
	.arena_proto = {
		.name = "roots",
		.size = sizeof(hz_root),
	}
};
hz_node *hz_root_new(hz_arena *arena)
{
	return hz_node_new(arena, hz_root, &hz_root_proto);
}
