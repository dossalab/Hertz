#version 100

uniform mat4 MVP;
uniform mat4 model;
uniform float time;

attribute vec3 in_position;
attribute vec3 in_normal;
attribute vec2 in_texcoords;

varying vec3 normal;
varying vec3 vertex;
varying vec2 texcoord;
varying float _time;

#define PI 3.1415926538

void main()
{
	vec2 resolution = vec2(4.0, 4.0);
	vec2 cPos = -1.0 + 2.0 * in_position.xy / resolution.xy;
	float cLength = length(cPos);

	vec2 uv = in_position.xy/resolution.xy+(cPos/cLength)*cos(cLength*12.0-time*1.0)*0.2;

	vec3 off = vec3(uv.x, cos(uv.x), uv.y);

	gl_Position = MVP * vec4(off + in_position, 1.0);
	vertex = vec3(model * vec4(in_position, 1.0));

	normal = normalize(in_normal);
	texcoord = in_texcoords;
	_time = time;
}

