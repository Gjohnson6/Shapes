#version 410 core

out vec2 TexCoord;

void main(void)
{
	const vec4 v[4] = vec4[4]
	(
		vec4(-1.0, -1.0, 0.0, 1.0),
		vec4( 1.0, -1.0, 0.0, 1.0),
		vec4( 1.0,  1.0, 0.0, 1.0),
		vec4(-1.0,  1.0, 0.0, 1.0)
	);

	const vec2 t[4] = vec2[4]
	(
		vec2(0.0, 0.0),
		vec2(1.0, 0.0),
		vec2(1.0, 1.0),
		vec2(0.0, 1.0)
	);

	TexCoord = t[gl_VertexID].yx;
	gl_Position = v[gl_VertexID];
}
