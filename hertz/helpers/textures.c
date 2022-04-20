#include GL_EXTENSIONS_HEADER
#include <hz/helpers/textures.h>

GLuint hz_create_texture(void *texture_data, GLenum format, size_t w, size_t h)
{
	GLuint texture;
	GLuint repeat_mode;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	/* Prevent interpolation artefacts on transparent textures */
	if (format == GL_RGBA) {
		repeat_mode = GL_CLAMP_TO_EDGE;
	} else {
		repeat_mode = GL_REPEAT;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat_mode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat_mode);

	glTexImage2D(GL_TEXTURE_2D, 0, format,
			w, h, 0, format, GL_UNSIGNED_BYTE, texture_data);

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return texture;
}
