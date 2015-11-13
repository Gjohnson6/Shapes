#version 410 core
//#extension GL_ARB_explicit_uniform_location : enable

// Output
layout (location = 0) out vec4 FragColor;

subroutine vec4 color_t();
subroutine uniform color_t ColorMode;

// Input from vertex shader - this must correspond exactly
// to the outputs of the vertex shader. The semantics of
// these are the same, however the values have been rasterized
// across the face of what ever it is we're drawing now.

in VS_OUT
{
    vec3 N;
    vec3 P;
	vec4 C;
	vec2 T;
} fs_in;

// Material properties
uniform vec3 diffuse_albedo;
uniform vec3 specular_albedo;
uniform float specular_power;
uniform vec3 ambient;

// Default position of light is taken to be in eye coordinates.
uniform vec3 light_position = vec3(0.0, 0.0, 1000.0);

uniform sampler2D base_texture;

// A global time value as needed by many of the shader toy shaders.
uniform float global_time;

subroutine(color_t)
vec4 Constant()
{
	return vec4(ambient, 1.0);
}

subroutine(color_t)
vec4 PerPixelLighting()
{
	vec3 diffuse2 = diffuse_albedo;
	vec3 N2 = fs_in.N;

	if (!gl_FrontFacing)
	{
		diffuse2 = vec3(fs_in.C);
		N2 = -N2;
	}
	vec3 n = normalize(N2);
	vec3 s = normalize(light_position - fs_in.P);
	vec3 v = normalize(-fs_in.P);
	vec3 r = reflect(-s, n);
	vec3 diffuse = max(dot(s, n), 0.0) * diffuse2;
	vec3 specular = pow(max(dot(r, v), 0.0), specular_power) * specular_albedo;

	return vec4(ambient + diffuse + specular, 1.0);
}

subroutine(color_t)
vec4 PPLWithTexture()
{
	vec3 diffuse2 = vec3(texture(base_texture, fs_in.T));
	vec3 N2 = fs_in.N;

	if (!gl_FrontFacing)
	{
		N2 = -N2;
	}
	vec3 n = normalize(N2);
	vec3 s = normalize(light_position - fs_in.P);
	vec3 v = normalize(-fs_in.P);
	vec3 r = reflect(-s, n);
	vec3 diffuse = max(dot(s, n), 0.0) * diffuse2;
	vec3 specular = pow(max(dot(r, v), 0.0), specular_power) * specular_albedo;

	return vec4(ambient + diffuse + specular, 1.0);
		//vec4(fs_in.T.t, fs_in.T.t, fs_in.T.t, 0); // 
}

subroutine(color_t)
vec4 ShaderToy1()
{
//	// http://www.pouet.net/prod.php?which=57245
//
//	vec3 c;
//	float l;
//	float z= global_time;
//
//	for(int i=0;i<3;i++)
//	{
//		vec2 p = fs_in.T;
//		vec2 uv = p;
//		p -= 0.5;
//		z +=.07;
//		l = length(p);
//		uv += p / l * (sin(z)+1.0) * abs(sin(l * 9.0 - z * 2.0));
//		c[i] = 0.01/length(abs(mod(uv, 1.0) - 0.5));
//	}
//	return vec4(c / l, 1.0);
// https://www.shadertoy.com/view/MdBGDK
// By David Hoskins.
    // Found this on GLSL sandbox. I really liked it, changed a few things and made it tileable.
// :)
// by David Hoskins.


// Water turbulence effect by joltz0r 2013-07-04, improved 2013-07-07

    //#define TAU 6.28318530718
    float time = global_time * .5+23.0;
    // uv should be the 0-1 uv of texture...
    vec2 uv = fs_in.T;
   

    vec2 p = mod(uv * 6.28318530718, 6.28318530718)-250.0;
    vec2 i = vec2(p);
    float c = 1.0;
    float inten = .005;

    for (int n = 0; n < 5; n++)
    {
        float t = time * (1.0 - (3.5 / float(n+1)));
        i = p + vec2(cos(t - i.x) + sin(t + i.y), sin(t - i.y) + cos(t + i.x));
        c += 1.0 / length(vec2(p.x / (sin(i.x + t) / inten), p.y / (cos(i.y + t) / inten)));
    }
    c /= float(5);
    c = 1.17-pow(c, 1.4);
    vec3 colour = vec3(pow(abs(c), 8.0));
    colour = clamp(colour + vec3(0.0, 0.35, 0.5), 0.0, 1.0);
   
    return vec4(colour / 1, 1.0);
}

void main(void)
{
	FragColor = ColorMode();
}
