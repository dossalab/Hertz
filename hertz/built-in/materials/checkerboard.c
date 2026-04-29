#include HZ_GL_HEADER
#include <hz/built-in/materials/checkerboard.h>
#include <stdint.h>

static void generate_checkerboard(uint8_t *pixels, int cells)
{
	for (int y = 0; y < cells; y++) {
		for (int x = 0; x < cells; x++) {
			int idx = (y * cells + x) * 3;
			uint8_t color = ((x + y) % 2 == 0) ? 0xFF : 0x40;

			pixels[idx + 0] = color;
			pixels[idx + 1] = color;
			pixels[idx + 2] = color;
		}
	}
}

hz_material *hz_checkerboard_material_new(hz_arena *arena, int cells)
{
	hz_material *m;
	uint8_t pixels[cells * cells * 3];

	if (cells < 1)
		return NULL;

	m = hz_material_new(arena);
	if (!m) {
		return NULL;
	}

	generate_checkerboard(pixels, cells);
	hz_material_bind_texture(m, HZ_TEXTURE_DIFFUSE, pixels,
			GL_RGB, cells, cells, false);

	return m;
}
