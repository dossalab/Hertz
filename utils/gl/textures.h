#ifndef UTILS_GL_TEXTURES_H
#define UTILS_GL_TEXTURES_H

#include <GL/glew.h>
#include <stddef.h>

GLuint create_texture_from_rgb(void *texture_data, size_t w, size_t h);

#endif
