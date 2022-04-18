#version 100

precision highp float;

uniform sampler2D tex;
varying vec3 _position;
varying vec3 _normal;
varying vec3 _uv;
varying vec3 _vertex;

void main()
{
	vec3 light = vec3(100, 100, 100);
	vec3 dir = normalize(light - _vertex);
	float diffuse = max(dot(normalize(_normal), dir), 0.0);
	float ambient = 0.2;

	float lighting = diffuse + ambient;

	vec3 color = texture2D(tex, _uv.xy).xyz;

	gl_FragColor = vec4(color * lighting, 1.0);
}

