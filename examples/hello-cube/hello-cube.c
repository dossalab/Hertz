#include <hz/util/logger.h>
#include <hz/contexts/glfw.h>
#include <hz/helpers/shaders.h>
#include <hz/built-in/shaders/simple.h>
#include <hz/built-in/primitives/cube.h>
#include <hz/material.h>
#include <stdlib.h>
#include <hz/units.h>

static const char *tag = "hcube";

static unsigned load_shaders(void)
{
	char *logs;
	unsigned shader;

	shader = hz_create_program_from_source(
				hz_simple_vertex_shader_source,
				hz_simple_fragment_shader_source, &logs);

	if (!shader) {
		hz_log_e(tag, "shader program compilation failed!");
		if (logs) {
			hz_log_e(tag, "compilation logs:\n%s", logs);
			free(logs);
		}

		return false;
	}

	return shader;
}

static bool init(hz_context *context, hz_arena *arena, hz_node *root, int *shader, void *user)
{
	*shader = load_shaders();

	hz_material *material = hz_material_new(arena, *shader);
	hz_node *cube = hz_cube_new(arena, *shader, material);

	hz_node_rotate(cube, HZ_DEG_TO_RAD(45.f), (hz_vec3){0.f, 1.f, 1.f});
	hz_node_insert(root, cube);

	return (*shader)? true : false;
}

int main(void)
{
	hz_logger_init(HZ_LOGLEVEL_DEBUG);
	return hz_context_wrapper(hz_glfw_context_new, init, NULL);
}
