#ifndef HZ_MATERIAL_H
#define HZ_MATERIAL_H

#include HZ_GL_HEADER
#include <stdbool.h>
#include <stddef.h>
#include <hz/types.h>

enum hz_texture_type {
	HZ_TEXTURE_DIFFUSE,
	HZ_TEXTURE_NORMAL,
	HZ_TEXTURE_SPECULAR,
};

struct hz_material {
	struct {
		GLuint diffuse, normal, specular, dummy;
	} textures;

	GLuint program;
};

bool hz_material_bind_texture(struct hz_material *m, enum hz_texture_type type,
		void *data, GLenum format, size_t w, size_t h);

void hz_material_use(struct hz_material *m);
bool hz_material_init(struct hz_material *m, GLuint program);

#endif
