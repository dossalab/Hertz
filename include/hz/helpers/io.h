#ifndef UTILS_GL_IO_H
#define UTILS_GL_IO_H

#include <GL/gl.h>

GLuint hz_create_shader_from_file(const char *filename, GLenum type);
GLuint hz_create_program_from_files(const char *vert_path, const char *frag_path);

#endif
