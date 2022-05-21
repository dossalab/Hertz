#include <hz/objects/root.h>
#include <internal/alloc.h>
#include <internal/object.h>
#include <stdbool.h>
#include <hz/utils/container_of.h>

struct hz_root {
	struct hz_object super;
};

static void root_bind(struct hz_object *super)
{
	/* pass */
}

static void root_draw(struct hz_object *super, struct hz_camera *camera)
{
	/* pass */
}

const struct hz_object_proto hz_root_proto = {
	.bind = root_bind,
	.draw = root_draw,
};

static bool root_init(struct hz_root *r)
{
	struct hz_object *super = HZ_OBJECT(r);
	hz_object_init(super, &hz_root_proto);
	return true;
}

struct hz_root *HZ_ROOT(struct hz_object *o)
{
	return hz_container_of(o, struct hz_root, super);
}

struct hz_object *hz_root_new(void)
{
	return HZ_OBJECT(hz_alloc_and_init(struct hz_root, root_init));
}
