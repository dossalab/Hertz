#include <hz/built-in/shaders/wireframe.h>
#include <hz/internal/program.h>

#define GLSL_VERSION	"100"

static const char *wireframe_vertex_shader_source =
	"#version " GLSL_VERSION "\n"
	"\n"
	"uniform mat4 MVP;\n"
	"\n"
	"attribute vec3 position;\n"
	"attribute vec3 barycentric;\n"
	"\n"
	"varying vec3 _barycentric;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	_barycentric = barycentric;\n"
	"	gl_Position = MVP * vec4(position, 1.0);\n"
	"}\n"
;

static const char *wireframe_fragment_shader_source =
	"#version " GLSL_VERSION "\n"
	"#extension GL_OES_standard_derivatives : enable\n"
	"\n"
	"precision highp float;\n"
	"\n"
	"varying vec3 _barycentric;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	vec3 d = fwidth(_barycentric);\n"
	"	vec3 a = smoothstep(vec3(0.0), d * 1.5, _barycentric);\n"
	"	float edge = min(min(a.x, a.y), a.z);\n"
	"	gl_FragColor = vec4(vec3(1.0 - edge), 1.0);\n"
	"}\n"
;

hz_program *hz_wireframe_program_new(hz_arena *arena)
{
	hz_program *p = hz_program_new(arena,
			wireframe_vertex_shader_source,
			wireframe_fragment_shader_source);
	return p;
}
