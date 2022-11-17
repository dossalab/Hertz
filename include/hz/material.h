#pragma once

#include HZ_GL_HEADER
#include <stdbool.h>
#include <stddef.h>
#include <hz/types.h>
#include <hz/util/arena.h>

typedef enum {
	HZ_TEXTURE_DIFFUSE,
	HZ_TEXTURE_NORMAL,
	HZ_TEXTURE_SPECULAR,
} hz_texture_type;

typedef struct _hz_material hz_material;

bool hz_material_bind_texture(hz_material *m, hz_texture_type type,
		void *data, GLenum format, size_t w, size_t h);

void hz_material_use(hz_material *m);
hz_material *hz_material_new(hz_arena *arena, GLuint program);
