#version 410 core

// Per-vertex attributes. The location here corresponds
// to the attribute number in the calling CPU-based program.
layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 textures;

uniform mat4 mv_matrix;
uniform mat4 view_matrix;
uniform mat4 proj_matrix;
uniform mat3 normal_matrix;
uniform float global_time;

// These are outputs of the vertex shader. They will
// arrive as inputs to the fragment shader after having
// been rasterized. This syntax (in particular) bundles
// the enclosed outs into what amounts to a struct.
out VS_OUT
{
    vec3 N;
    vec3 P;
	vec4 C;
	vec2 T;
} vs_out;


void main(void)
{
	vs_out.N = normalize(inverse(transpose(mat3(mv_matrix))) * normal);
	vs_out.P = vec3(mv_matrix * vec4(position, 1.0));
	vs_out.C = color;
	vs_out.T = vec2(textures.x, 1.0 - textures.y);
	
	gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
}
