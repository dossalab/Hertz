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

	vec3 light = vec3(1000, 0, 0);
	vec3 dir = normalize(light - vertex);
	float diffuse = max(dot(normal, dir), 0.0);
	float ambient = 0.2; //0.3 * sin(time / 100.0) + 0.7;
	vec3 color = vec3(sin(time / 30.0) / time / 30.0, 1.0, sin(time / 30.0));

	vec3 lighting = color * (diffuse + ambient);

	vec2 resolution = vec2(1.0, 1.0);
	vec2 cPos = -1.0 + 2.0 * texcoord.xy / resolution.xy;
	float cLength = length(cPos);

	vec2 uv = texcoord.xy/resolution.xy+(cPos/cLength)*cos(cLength*12.0-time*0.2)*0.03;

	vec3 col = texture2D(tex,uv).xyz;

	gl_FragColor = vec4(col, 1.0) * vec4(lighting, 1.0);
}

