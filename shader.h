#pragma once

/*
A shader support BASE class.

Perry Kivolowitz - University of Wisconsin - Madison
Computer Sciences Department

Created:	2/25/13
Updates:	10/4/2015		Rolling into generic shape builders.

*/

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ilcontainer.h"
#include "fbo.h"

class Shader
{
public:
	Shader();
	void TakeDown();
	void Use();
	void UnUse();
	virtual bool Initialize(std::string vertex_shader_file, std::string fragment_shader_file);
	virtual void CustomSetup() = 0;
	virtual void SetMaterial(glm::vec3 diffuse_albedo, glm::vec3 specular_albedo, float specular_power, glm::vec3 ambient) = 0;
	virtual void SetLightPosition(glm::vec3 light_position) {}
	virtual void SelectSubroutine(int subroutine_index) {}
	virtual void SetGlobalTime(float global_time) { }
	virtual void EnableTexture(ILContainer & ilcontainer , GLuint texture_unit) {}

	GLuint vertex_shader_id;
	GLuint fragment_shader_id;
	GLuint program_id;
	bool LoadShader(std::string file_name, GLuint shader_id);
	std::stringstream GetShaderLog(GLuint shader_id);
	bool GLReturnedError(char * s);

protected:
	bool is_used;

private:
};

class ShaderInitializer
{
public:
	ShaderInitializer(Shader * shader, char * vertex_shader_file_name, char * fragment_shader_file_name, char * error_string) :
		shader(shader), vertex_shader_file_name(vertex_shader_file_name), fragment_shader_file_name(fragment_shader_file_name), error_string(error_string)
	{}

	static bool Initialize(std::vector<ShaderInitializer> * shaders);
	static void TakeDown(std::vector<ShaderInitializer> * shaders);

	Shader * shader;
	char * vertex_shader_file_name;
	char * fragment_shader_file_name;
	char * error_string;
};

