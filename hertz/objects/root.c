#include <hz/objects/root.h>

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

void hz_root_init(struct hz_root *r)
{
	struct hz_object *super = HZ_OBJECT(r);
	hz_object_init(super, &hz_root_proto);
}
