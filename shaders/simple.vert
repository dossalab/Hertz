#version 100

uniform mat4 MVP;
uniform mat4 model;

attribute vec3 position;
attribute vec3 normal;
attribute vec3 uv;

varying vec3 _position;
varying vec3 _normal;
varying vec3 _uv;
varying vec3 _vertex;

void main()
{
	gl_Position = MVP * vec4(position, 1.0);
	_vertex = vec3(model * vec4(position, 1.0));

	_normal = normal;
	_uv = uv;
}

