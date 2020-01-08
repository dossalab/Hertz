#version 100

uniform mat4 MVP;
uniform mat4 model;

attribute vec3 in_position;
attribute vec3 in_normal;
attribute vec2 in_texcoords;

varying vec3 normal;
varying vec3 vertex;
varying vec2 texcoord;

void main()
{
	gl_Position = MVP * vec4(in_position, 1.0);
	vertex = vec3(model * vec4(in_position, 1.0));
	normal = normalize(in_normal);
	texcoord = in_texcoords;
}

