#pragma once
#include "shader.h"

#define	BOX_FILTER

class BlurShader : public Shader
{
public:
	void UnUse();
	void SetMaterial(glm::vec3 diffuse_albedo , glm::vec3 specular_albedo , float specular_power , glm::vec3 ambient) { };
	void CustomSetup();

	static void Blur(BlurShader & bs , FrameBufferObject & fbo , bool copy_back , int image_size , int number_of_samples);
private:
	struct
	{
		GLuint image_size;
		GLuint number_of_samples;
		GLuint texture;
	} uniforms;
	int image_size;
	int number_of_samples;
};
