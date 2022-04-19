#include <hz/loader.h>

bool hz_loader_import_scene(const char *path, struct hz_scene *s)
{
	return hz_default_loader_proto->import_scene(path, s);
}
