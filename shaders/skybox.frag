#version 100

precision highp float;

uniform sampler2D tex;
varying vec3 normal;
varying vec3 vertex;
varying vec3 texcoord;

void main()
{
	float ambient = 0.8;

	vec3 color = vec3(0.7, 0.7, 0.7);// texture2D(tex, texcoord).xyz;

	gl_FragColor = vec4(color * ambient, 1.0);
}

