#include HZ_GL_EXTENSIONS_HEADER
#include <stdlib.h>
#include <hz/internal/program.h>
#include <hz/util/arena.h>
#include <hz/util/logger.h>
#include <hz/internal/helpers/shaders.h>

static const char *tag = "prog";

static void program_free(void *mem)
{
	hz_program *p = mem;
	glDeleteProgram(p->id);
}

static const hz_arena_proto hz_program_arena_proto = {
	.name = "programs",
	.free = program_free,
	.size = sizeof(hz_program),
};

hz_program *hz_program_new(hz_arena *arena, const char *vert, const char *frag)
{
	hz_program *p = hz_arena_alloc(arena, &hz_program_arena_proto);
	char *logs = NULL;

	p->id = hz_create_program_from_source(vert, frag, &logs);
	if (!p->id) {
		if (logs) {
			hz_log_e(tag, "shader compilation failed:\n%s", logs);
			free(logs);
		}
		return NULL;
	}

	return p;
}
