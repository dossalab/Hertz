#include <hz/loader.h>

bool hz_loader_import_scene(const char *path, struct hz_object *root)
{
	return hz_default_loader_proto->import_scene(path, root);
}
