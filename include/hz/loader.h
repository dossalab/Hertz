#ifndef HZ_LOADER_H
#define HZ_LOADER_H

#include <stdbool.h>
#include <hz/object.h>

struct hz_loader_proto {
	bool (*import_scene)(const char *, struct hz_object *);
};

#define hz_export_loader_proto(x) \
	struct hz_loader_proto *hz_default_loader_proto = &(x)

extern struct hz_loader_proto *hz_default_loader_proto;

bool hz_loader_import_scene(const char *path, struct hz_object *root);

#endif
