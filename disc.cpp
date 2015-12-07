#include <iostream>
#include <stdexcept>
#include "disc.h"

using namespace std;
using namespace glm;

Disc::Disc(int slices, float span, float outer_radius, float inner_radius) : Shape()
{
	float full_span = pi<float>() * 2.0f;

	if (span == 0)
		throw std::invalid_argument("bad span value");
	
	this->slices = abs(slices);
	this->span = min<float>(full_span, abs(span));
	this->inner_radius = inner_radius;
	this->outer_radius = outer_radius;

	this->is_partial_span = this->span != full_span;
}

bool Disc::PreGLInitialize()
{
	this->is_fan = false;

	float scale_factor_for_normals = 1.0f / 10.0f;
	int real_number_of_slices = this->slices + (this->is_partial_span ? 1 : 0);

	// If the inner radius is greater than zero, two rings of vertices will be created.
	// The inner and outer ring will be connected via two triangles per slice.
	//
	// If the inner radius is zero, a triangle fan will be created.

	this->data.vertices.reserve(real_number_of_slices + (this->inner_radius == 0.0f ? 1 : real_number_of_slices));
	this->data.normals.reserve(real_number_of_slices + (this->inner_radius == 0.0f ? 1 : real_number_of_slices));

	mat4 m;
	vec4 p(this->outer_radius, 0.0f, 0.0f, 1.0f);
	vec3 n(0.0f, 0.0f, 1.0f);
	vec3 z(0.0f, 0.0f, 1.0f);

	float theta = this->span / float(this->slices);

	if (this->inner_radius == 0.0)
	{
		this->is_fan = true;
		// Add center as first point so that triangle fan can be used.
		this->data.vertices.push_back(vec3(0.0f, 0.0f, 0.0f));
		this->data.textures.push_back(vec2(0.5f , 0.5f));
		this->data.colors.push_back(this->RandomColor(vec4(0.5f , 0.5f , 0.5f , 1.0f)));
		this->data.normals.push_back(n);
		this->data.normal_visualization_coordinates.push_back(*(data.vertices.end() - 1));
		this->data.normal_visualization_coordinates.push_back(n / 8.0f);
	}

	// An outer ring is required in all cases.
	for (int i = 0; i < real_number_of_slices; i++)
	{
		this->data.vertices.push_back(vec3(m * p));
		this->data.textures.push_back(vec2(*(this->data.vertices.end() - 1)) / (this->outer_radius * 2.0f) + vec2(0.5f , 0.5f));
		this->data.normals.push_back(n);
		this->data.colors.push_back(this->RandomColor(vec4(0.5f , 0.5f , 0.5f , 1.0f) , -0.3f , 0.3f));
		this->data.normal_visualization_coordinates.push_back(*(data.vertices.end() - 1));
		this->data.normal_visualization_coordinates.push_back(*(data.vertices.end() - 1) + n / 8.0f);
		m = rotate(m, theta, z);
	}

	if (this->inner_radius == 0.0)
	{
		// This is THE only time a TRIANGLE_FAN works.
		for (unsigned int i = 0; i < this->data.vertices.size(); i++)
		{
			this->data.indices.push_back(i);
		}
		if (!this->is_partial_span)
			this->data.indices.push_back(1);
	}
	else
	{
		m = mat4();
		p = vec4(this->inner_radius, 0.0f, 0.0f, 1.0f);

		// Instanciate the second ring.
		for (int i = 0; i < real_number_of_slices; i++)
		{
			this->data.vertices.push_back(vec3(m * p));
			this->data.textures.push_back(vec2(p) / (this->inner_radius * 2.0f) + vec2(0.5f , 0.5f));
			this->data.normals.push_back(n);
			this->data.colors.push_back(this->RandomColor(vec4(0.5f, 0.5f, 0.5f, 1.0f)));
			this->data.normal_visualization_coordinates.push_back(*(data.vertices.end() - 1));
			this->data.normal_visualization_coordinates.push_back(*(data.vertices.end() - 1) + n / 8.0f);
			m = rotate(m, theta, z);
		}

		// There will be two triangles per slice.
		// First is: current outer, current inner, next outer
		// Second is: next outer, current inner, next inner
		for (int i = 0; i < this->slices; i++)
		{
			// First
			this->data.indices.push_back(i);
			this->data.indices.push_back(real_number_of_slices + i);
			this->data.indices.push_back((i + 1) % real_number_of_slices);
			// Second
			this->data.indices.push_back((i + 1) % real_number_of_slices);
			this->data.indices.push_back(real_number_of_slices + i);
			this->data.indices.push_back(real_number_of_slices + (i + 1) % real_number_of_slices);
		}
	}
	this->data.vbackup = this->data.vertices;
	return true;
}

void Disc::NonGLTakeDown()
{

}

void Disc::RecomputeNormals()
{
	vec3 sum;
	float denominator;
	vector<vec3> & p = this->data.normal_visualization_coordinates;
	vector<vec3> & v = this->data.vertices;
	vector<vec3> & n = this->data.normals;

	vec3 A;
	vec3 B;
	if (this->inner_radius == 0.0f)
	{
		// Processing for the central vertex only.
		for (unsigned int i = 0; i < v.size() - 1; i++)
		{
			if (i != data.vertices.size() - 2)
			{
				//cross product between each set of verticies and find average
				A = (v[0] - v[i + 2]);
				B = (v[0] - v[i + 1]);
			}
			else if (!this->is_partial_span)
			{
				A = (v[0] - v[1]);
				B = (v[0] - v[i + 1]);
			}
			sum += normalize(cross(normalize(B) , normalize(A)));
		}
		n[0] = -sum / float(v.size() - (this->is_partial_span ? 2 : 1));
		p[0] = v[0];
		p[1] = v[0] + n[0] / 8.0f;

		for (unsigned int i = 1; i < v.size(); i++)
		{	
			sum = vec3();
			float points = 0.0f;
			if ((i != v.size() - 1 && is_partial_span) || !is_partial_span)
			{
				A = (v[i] - v[0]);
				B = (v[i] - v[(i == v.size() - 1 ? 1 : i + 1)]);
				sum += normalize(cross(normalize(B) , normalize(A)));
				points++;
			}
			if ((i != 1 && is_partial_span) || !is_partial_span)
			{
				A = (v[i] - v[(i == 1 ? v.size() : i) - 1]);
				B = (v[i] - v[0]);
				sum += normalize(cross(normalize(B) , normalize(A)));
				points++;
			}

			n[i] = -sum / points;
			p[i * 2 + 0] = v[i];
			p[i * 2 + 1] = v[i] + n[i] / 8.0f;
		}
	}
	else
	{
		// Outer verticies
		for (int i = 0; i < this->slices; i++)
		{
			if (this->is_partial_span)
			{
				// When the disk is a partial span, the first and last
				// slices but be considered differently. 
				if (i == 0)
				{
					// The first outer vertex consists of a single triangle.
					denominator = 1.0f;
					A = normalize(v[i + 1] - v[i]);
					B = normalize(v[i + this->slices] - -v[i]);
					sum = normalize(cross(B , A));
				}
				else
				{
					denominator = 2.0;
					A = normalize(v[i + this->slices - 1] - v[i]);
					B = normalize(v[i - 1] - -v[i]);
					sum = normalize(cross(B , A));
					A = normalize(v[i + this->slices] - v[i]);
					B = normalize(v[i + this->slices - 1] - -v[i]);
					sum += normalize(cross(B , A));
				}
				n[i] = -sum / denominator;
				p[i * 2 + 0] = v[i];
				p[i * 2 + 1] = v[i] + n[i] / 8.0f;
			}
		}
	}
}

void Disc::Draw(bool draw_normals)
{
	this->GLReturnedError("Disc::Draw() - entering");

	if (this->data.vertices.size() == 0)
	{
		this->PreGLInitialize();
		this->CommonGLInitialization();
	}

	if (draw_normals)
	{
		glBindVertexArray(this->normal_array_handle);
		glDrawArrays(GL_LINES, 0, this->data.normal_visualization_coordinates.size());
	}
	else
	{
		GLint winding;
		glGetIntegerv(GL_FRONT_FACE , &winding);
		if (is_fan)
			glFrontFace(GL_CCW);
		glBindVertexArray(this->vertex_array_handle);
		glDrawElements(this->is_fan ? GL_TRIANGLE_FAN : GL_TRIANGLES, this->data.indices.size(), GL_UNSIGNED_INT, nullptr);
		glFrontFace(winding);
	}
	glBindVertexArray(0);
	this->GLReturnedError("Disc::Draw() - exiting");
}