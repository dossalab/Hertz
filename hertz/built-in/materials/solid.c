#include HZ_GL_HEADER
#include <hz/built-in/materials/solid.h>
#include <stdint.h>

hz_material *hz_solid_material_new(hz_arena *arena, float r, float g, float b, float shininess)
{
	uint8_t pixel[3] = {
		(uint8_t)(r * 255.f),
		(uint8_t)(g * 255.f),
		(uint8_t)(b * 255.f),
	};

	hz_material *m = hz_material_new(arena);

	hz_material_bind_texture(m, HZ_TEXTURE_DIFFUSE, pixel, GL_RGB, 1, 1, false);
	hz_material_set_shininess(m, shininess);

	return m;
}
