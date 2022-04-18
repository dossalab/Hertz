#ifndef OBJECTS_BASIC_H
#define OBJECTS_BASIC_H

#include <GL/gl.h>
#include <utils/3rdparty/linmath/linmath.h>
#include <stdbool.h>
#include <stddef.h>
#include <render/object.h>
#include <utils/common.h>

struct basic_object {
	struct object as_object;

	struct {
		GLuint vertices, normals, uvs, indices;
	} buffers;

	struct {
		GLint mvp, model;
	} uniforms;

	struct {
		mat4x4 model;
		mat4x4 mvp;
	} transform;

	GLuint program;
	GLuint texture;

	bool texture_attached;
	size_t nindices;
};

#define cast_basic_object(ptr) \
	container_of(ptr, struct basic_object, as_object)

bool basic_object_texture(struct basic_object *o, GLuint texture,
		vec3 *uvs, size_t uv_count);

bool basic_object_create_from_geometry(struct basic_object *o, GLuint shader_program,
		vec3 *vertices, vec3 *normals, size_t nvertices,
		unsigned *indices, size_t nindices);

#endif
