#ifndef GL_TEXTURES_H
#define GL_TEXTURES_H

#include <GL/glew.h>
#include <stdint.h>
#include <stddef.h>

GLuint create_texture_from_rgb(uint8_t *texture_data, size_t w, size_t h);

#endif