#include HZ_GL_EXTENSIONS_HEADER
#include <internal/alloc.h>
#include <hz/material.h>
#include <vendor/linmath/linmath.h>
#include <hz/helpers/textures.h>

#define HZ_DIFFUSE_LOCATION	GL_TEXTURE0
#define HZ_NORMAL_LOCATION	GL_TEXTURE1
#define HZ_SPECULAR_LOCATION	GL_TEXTURE2

static GLuint create_dummy_texture(void)
{
	uint32_t data = 0xFFFFFF;
	return hz_create_texture(&data, GL_RGB, 1, 1);
}

void hz_material_use(struct hz_material *m)
{
	glActiveTexture(HZ_DIFFUSE_LOCATION);
	glBindTexture(GL_TEXTURE_2D, m->textures.diffuse);
	glActiveTexture(HZ_NORMAL_LOCATION);
	glBindTexture(GL_TEXTURE_2D, m->textures.normal);
	glActiveTexture(HZ_SPECULAR_LOCATION);
	glBindTexture(GL_TEXTURE_2D, m->textures.specular);
}

bool hz_material_bind_texture(struct hz_material *m, enum hz_texture_type type,
		void *data, GLenum format, size_t w, size_t h)
{
	GLuint *texture;

	switch (type) {
	case HZ_TEXTURE_DIFFUSE:
		texture = &m->textures.diffuse;
		break;

	case HZ_TEXTURE_NORMAL:
		texture = &m->textures.normal;
		break;

	case HZ_TEXTURE_SPECULAR:
		texture = &m->textures.normal;
		break;

	default:
		return false;
	}

	*texture = hz_create_texture(data, format, w, h);
	return !!(*texture);
}

static bool material_init(struct hz_material *m, GLuint program)
{
	GLuint dummy;

	dummy = create_dummy_texture();
	if (!dummy) {
		return false;
	}

	m->program = program;

	m->textures.dummy = dummy;
	m->textures.diffuse = dummy;
	m->textures.normal = dummy;
	m->textures.specular = dummy;

	return true;
}

static void delete_texture_if_bound(struct hz_material *m, GLuint texture)
{
	if (m->textures.dummy != texture) {
		glDeleteTextures(1, &texture);
	}
}

void hz_material_deinit(struct hz_material *m)
{
	delete_texture_if_bound(m, m->textures.diffuse);
	delete_texture_if_bound(m, m->textures.normal);
	delete_texture_if_bound(m, m->textures.specular);

	glDeleteTextures(1, &m->textures.dummy);
}

struct hz_material *hz_material_new(GLuint program)
{
	return hz_alloc_and_init(struct hz_material, material_init, program);
}
