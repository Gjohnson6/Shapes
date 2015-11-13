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
	vec2 texture(-1.0f, 1.0f);

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

		vec2 tc(j / float(this->rings), 0.0f);

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
			texture.x = texture.x + dpsi;
		}

		texture.x = -1.0f;
		texture.y = texture.y + dphi;
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
	vector<vec3> & v = data.vertices;
	vector<vec3> & n = data.normals;
	vector<vec3> & p = data.normal_visualization_coordinates;
	vec3 A;
	vec3 B;
	vec3 C;
	vec3 D;
	vec3 sum;
	int index = 0;
	for (int j = 0; j < this->rings; j++)
	{

		for (int i = 0; i < this->sides; i++, index++)
		{
			if (j != this->rings - 1)
			{
				if (i == this->sides - 1)//If it's the last vertex in a ring (which is the same position as the first)
				{
					if (index - this->sides < 0)
					{

					}
					n[index] = n[index - this->sides + 1];
					p[index * 2 + 0] = p[(index - this->sides + 1) * 2 + 0];
					p[index * 2 + 1] = p[(index - this->sides + 1) * 2 + 1];
				}
				else
				{
					sum = vec3(0.0f);
					int vert = (j * this->sides + i);
					int indexA = vert + 1;
					int indexB = (vert + this->sides);

					A = (v[indexA] - v[vert]);
					B = (v[indexB] - v[vert]);
					sum += normalize(cross(B, A ));

					indexB = indexA;
					indexA = vert - (this->sides) + 1;
					if (indexA < 0)
					{
						indexA = this->data.vertices.size() - this->sides + indexA;//Need to subtract this->sides to skip the last ring, which is the same as the first
						//can't use % because it works differently with negative numbers
					}

					A = (v[indexA] - v[vert]);
					B = (v[indexB] - v[vert]);
					sum += normalize(cross(A, B));

					indexB = indexA;
					indexA = vert - this->sides;
					if (indexA < 0)
					{
						indexA = this->data.vertices.size() - this->sides + indexA;																				   
					}

					A = (v[indexA] - v[vert]);
					B = (v[indexB] - v[vert]);
					sum += normalize(cross(A, B));
					indexB = indexA;
					indexA = i == 0 ? vert + this->sides - 2 : vert - 1;//this is the point to the 'left' of vert. If vert is 0, the point is sides - 2, otherwise it would be the last point

					A = (v[indexA] - v[vert]);
					B = (v[indexB] - v[vert]);
					sum += normalize(cross(A, B));

					indexB = indexA;
					indexA = i == 0 ? (vert + (this->sides * 2) - 2) : vert + this->sides - 1;

					A = (v[indexA] - v[vert]);
					B = (v[indexB] - v[vert]);
					sum += normalize(cross(A, B));

					indexB = indexA;
					indexA = (vert + this->sides);

					A = (v[indexA] - v[vert]);
					B = (v[indexB] - v[vert]);
					sum += normalize(cross(A, B));
					
					n[index] = - sum / 6.0f;
					p[index * 2 + 0] = v[index];
					p[index * 2 + 1] = v[index] + n[index] / - 8.0f;
				}
			}
			else//last ring is the same as the first
			{
				n[index] = n[i];
				p[index * 2 + 0] = p[i * 2 + 0];
				p[index * 2 + 1] = p[i * 2 + 1];
			}
		}
	
	}


}

