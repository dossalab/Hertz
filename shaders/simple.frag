#version 100

precision lowp float;

uniform sampler2D tex;
varying vec3 normal;
varying vec3 vertex;
varying vec2 texcoord;

void main()
{
	vec3 light = vec3(10, 10, 10);
	vec3 dir = normalize(light - vertex);
	float diffuse = max(dot(normal, dir), 0.0);
	float ambient = 0.2;
	vec3 color = vec3(1.0, 1.0, 1.0);

	vec3 lighting = color * (diffuse + ambient);

	vec4 tex_color = texture2D(tex, texcoord);

	gl_FragColor = tex_color * vec4(lighting, 1.0);
}

