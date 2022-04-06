#include "loader.h"

bool loader_import_scene(const char *path, struct scene *s)
{
	return default_loader_proto->import_scene(path, s);
}
