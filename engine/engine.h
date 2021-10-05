#ifndef ENGINE_ENGINE_H
#define ENGINE_ENGINE_H

#include <stddef.h>
#include <engine/camera.h>
#include <engine/scene.h>

#define ENG_PI		3.14159265359f
#define ENG_FOV		(70.0f * ENG_PI / 180.0f)

void engine_set_scene(struct scene *s);
void engine_set_camera(struct camera *c);
void engine_update_mvp(void);

void engine_handle_resize(size_t width, size_t height);
void engine_redraw(void);

#endif

