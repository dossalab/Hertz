#version 100

precision highp float;

uniform sampler2D tex;
varying vec3 normal;
varying vec3 vertex;
varying vec2 texcoord;
varying float _time;

void main()
{
	float time = _time;

	vec3 light = vec3(100, 100, 0);
	vec3 dir = normalize(light - vertex);
	float diffuse = max(dot(normalize(normal), dir), 0.0);
	float ambient = 0.2;

	float lighting = diffuse + ambient;

	vec3 color = texture2D(tex, texcoord).xyz;

	gl_FragColor = vec4(color * lighting, 1.0);
}

