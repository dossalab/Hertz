#ifndef HZ_HELPERS_TEXTURES_H
#define HZ_HELPERS_TEXTURES_H

#include <stddef.h>
#include <GL/gl.h>

GLuint hz_create_texture(void *texture_data, GLenum format, size_t w, size_t h);

#endif
