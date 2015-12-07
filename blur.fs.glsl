#version 410 core

layout (location = 0) out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D source_texture;

uniform int image_size;
uniform int number_of_samples;

vec4 blur_pass()
{
	float dx = 1.0 / image_size;
	float divisor = (2.0 * number_of_samples - 1);
	vec2 l = TexCoord, r = TexCoord;
	vec4 sum = texture2D(source_texture, TexCoord);

	if (number_of_samples == 1)
	{
		divisor = 1.5;
		l.x -= dx;
		r.x += dx;
		sum += texture2D(source_texture, l) * 0.25;
		sum += texture2D(source_texture, r) * 0.25;
	}
	else
	{
		for (int i = 1; i < number_of_samples; i++)
		{
			l.x -= dx;
			r.x += dx;
			sum += texture2D(source_texture, l);
			sum += texture2D(source_texture, r);
		}
	}
	return sum / divisor;
}

void main (void)
{
	FragColor = blur_pass();
}

