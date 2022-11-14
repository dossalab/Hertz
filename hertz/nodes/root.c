#include <hz/nodes/root.h>
#include <hz/internal/alloc.h>
#include <hz/internal/node.h>

struct _hz_root {
	hz_node super;
};

static void root_bind(hz_node *super)
{
	/* pass */
}

static void root_draw(hz_node *super, hz_camera *camera)
{
	/* pass */
}

const hz_node_proto hz_root_proto = {
	.bind = root_bind,
	.draw = root_draw,
};

static bool root_init(hz_root *r)
{
	hz_node *super = HZ_NODE(r);
	hz_node_init(super, &hz_root_proto);
	return true;
}

hz_root *HZ_ROOT(hz_node *n)
{
	return hz_container_of(n, hz_root, super);
}

hz_node *hz_root_new(void)
{
	return HZ_NODE(hz_alloc_and_init(hz_root, root_init));
}
