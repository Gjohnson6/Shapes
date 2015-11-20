#include "torus.h"
#include <vector>
#include <iostream>

using namespace glm;
using namespace std;

Torus::Torus(float innerRadius, float  outerRadius, int sides, int rings)
{
	if (innerRadius < 0 || sides < 3 || rings < 3)//Don't allow for either radius to be less than 0 or for the side or ring vlaues to be less than 3
	{
		throw std::invalid_argument("Bad value in Torus constructor");
	}
	this->inner_radius = innerRadius;
	this->outer_radius = outerRadius;
	this->sides = sides;
	this->rings = rings;
}

bool Torus::PreGLInitialize()
{
	//This code is adapted from freeglut's glutSolidTorus source
	//http://freeglut.sourcearchive.com/documentation/2.4.0-5.1ubuntu1/freeglut__geometry_8c-source.html
	float phi, psi;
	float dpsi, dphi;//difference values for psi and phi
	float spsi, cpsi;//sin and cos of psi
	float sphi, cphi;//sin and cos of phi
	vec3 vertex;
	vec3 normal;

	//Increment number of sides to allow for one more point than the surface
	sides++;
	rings++;

	dpsi = 2.0f * pi<float>() / float(rings - 1);
	dphi = -2.0f * pi<float>() / float(sides - 1);
	psi = 0.0f;

	for (int j = 0; j < rings; j++)
	{
		cpsi = cos(psi);
		spsi = sin(psi);
		phi = 0.0f;

		vec2 tc(j / float(this->rings - 1 ), 0.0f);//Texture coordinate

		for (int i = 0; i < sides; i++)
		{
			cphi = cos(phi);
			sphi = sin(phi);

			vertex = vec3(cpsi * (outer_radius + cphi * inner_radius), spsi * (outer_radius + cphi * inner_radius), sphi * inner_radius);
			normal = vec3(cpsi * cphi * -1, spsi * cphi * -1, sphi * -1);
			tc = vec2(tc.s, i / float(this->sides - 1));
			this->data.vertices.push_back(vertex);
			this->data.textures.push_back(tc);
			this->data.normals.push_back(normal);
			this->data.colors.push_back(this->RandomColor((this->data.colors.size() > 0 ? *(this->data.colors.end() - 1) : vec4(0.5f, 0.5f, 0.5f, 1.0f)), -0.2f, 0.2f));
			this->data.normal_visualization_coordinates.push_back(*(this->data.vertices.end() - 1));
			this->data.normal_visualization_coordinates.push_back(*(this->data.vertices.end() - 1) + *(this->data.normals.end() - 1) / this->NORMAL_LENGTH_DIVISOR * vec3(-1, -1, -1));

			phi += dphi;
		}

		psi += dpsi;
	}
	int w = sides;
	int verticesCount = this->data.vertices.size();

	for (int j = 0; j < this->rings - 1; j++)
	{
		for (int i = 0; i < this->sides; i++)
		{
			
			this->data.indices.push_back(j * w + i);
			this->data.indices.push_back(j * w + (i + 1) % w );
			this->data.indices.push_back((j + 1) * w + i );

			this->data.indices.push_back(j * w + (i + 1) % w);
			this->data.indices.push_back((j + 1) * w + (i + 1) % w);
			this->data.indices.push_back((j + 1) * w + i);
		}
	}

	this->data.vbackup = this->data.vertices;

	return true;
}

void Torus::NonGLTakeDown()
{
}

void Torus::RecomputeNormals()
{
}

