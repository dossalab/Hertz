#ifndef ASSETS_LOADER_H
#define ASSETS_LOADER_H

#include <stdbool.h>

struct hz_scene;

struct hz_loader_proto {
	bool (*import_scene)(const char *, struct hz_scene *);
};

#define hz_export_loader_proto(x) \
	struct hz_loader_proto *hz_default_loader_proto = &(x)

extern struct hz_loader_proto *hz_default_loader_proto;

bool hz_loader_import_scene(const char *path, struct hz_scene *s);

#endif
