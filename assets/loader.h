#ifndef ASSETS_LOADER_H
#define ASSETS_LOADER_H

#include <stdbool.h>

struct scene;

struct loader_proto {
	bool (*import_scene)(const char *, struct scene *);
};

#define export_loader_proto(x) \
	struct loader_proto *default_loader_proto = &(x)

extern struct loader_proto *default_loader_proto;

bool loader_import_scene(const char *path, struct scene *s);

#endif
