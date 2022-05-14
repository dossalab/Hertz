#include <hz/objects/root.h>

static void root_draw(struct hz_object *super, struct hz_camera *camera)
{
	/* pass */
}

const struct hz_object_proto hz_root_proto = {
	.draw = root_draw,
};

void hz_root_init(struct hz_root *r)
{
	struct hz_object *super = hz_cast_object(r);
	hz_object_init(super, &hz_root_proto);
}
