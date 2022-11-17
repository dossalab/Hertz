#include HZ_GL_EXTENSIONS_HEADER
#include <hz/util/logger.h>
#include <hz/material.h>
#include <vendor/linmath/linmath.h>
#include <hz/helpers/textures.h>

#define HZ_DIFFUSE_LOCATION	GL_TEXTURE0
#define HZ_NORMAL_LOCATION	GL_TEXTURE1
#define HZ_SPECULAR_LOCATION	GL_TEXTURE2

static const char *tag = "mtl";

struct _hz_material {
	struct {
		GLuint diffuse, normal, specular, dummy;
	} textures;

	GLuint program;
};

static GLuint create_dummy_texture(void)
{
	uint32_t data = 0xFFFFFF;
	return hz_create_texture(&data, GL_RGB, 1, 1);
}

void hz_material_use(hz_material *m)
{
	glActiveTexture(HZ_DIFFUSE_LOCATION);
	glBindTexture(GL_TEXTURE_2D, m->textures.diffuse);
	glActiveTexture(HZ_NORMAL_LOCATION);
	glBindTexture(GL_TEXTURE_2D, m->textures.normal);
	glActiveTexture(HZ_SPECULAR_LOCATION);
	glBindTexture(GL_TEXTURE_2D, m->textures.specular);
}

bool hz_material_bind_texture(hz_material *m, hz_texture_type type,
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

static void material_init(hz_material *m, GLuint program)
{
	GLuint dummy;

	dummy = create_dummy_texture();
	if (!dummy) {
		hz_log_e(tag, "unable to create dummy texture");
		return;
	}

	m->program = program;

	m->textures.dummy = dummy;
	m->textures.diffuse = dummy;
	m->textures.normal = dummy;
	m->textures.specular = dummy;
}

static void delete_texture_if_bound(hz_material *m, GLuint texture)
{
	if (m->textures.dummy != texture) {
		glDeleteTextures(1, &texture);
	}
}

static void material_free(void *mem)
{
	hz_material *m = mem;

	delete_texture_if_bound(m, m->textures.diffuse);
	delete_texture_if_bound(m, m->textures.normal);
	delete_texture_if_bound(m, m->textures.specular);

	glDeleteTextures(1, &m->textures.dummy);
}

static const hz_arena_proto hz_material_arena_proto = {
	.name = "materials",
	.free = material_free,
	.size = sizeof(hz_material),
};

hz_material *hz_material_new(hz_arena *arena, GLuint program)
{
	hz_material *material = hz_arena_alloc(arena, &hz_material_arena_proto);
	if (!material) {
		return NULL;
	}

	material_init(material, program);

	return material;
}
