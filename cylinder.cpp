#include <iostream>
#include <stdexcept>
#include "cylinder.h"

// Leave this defined 
#define	TRIANGLES

using namespace std;
using namespace glm;

#if 0

Cylinder::Cylinder(int slices, int stacks, float span, float back_radius, float front_radius) : Shape()
{
	float full_span = pi<float>() * 2.0f;

	if (span == 0)
		throw std::invalid_argument("bad span value");

	if (stacks < 1)
		throw std::invalid_argument("bad stack value");

	if (back_radius == 0.0f && front_radius == 0.0f)
		throw std::invalid_argument("bad radii");

	this->slices = abs(slices);
	this->stacks = stacks;
	this->fr = front_radius;
	this->br = back_radius;
	this->span = min<float>(full_span, abs(span));
	this->is_partial_span = this->span != full_span;
}

void Linear(float x, float * evaluation, float * derivitive, float fr, float br)
{
	assert(evaluation != nullptr);
	assert(derivitive != nullptr);

	*evaluation = mix(br, fr, x);
	*derivitive = normalize(fr - br);
}

void Quadratic(float x, float * evaluation, float * derivitive, float p1, float p2)
{
	assert(evaluation != nullptr);
	assert(derivitive != nullptr);

	*evaluation = mix(p1, p2, x * x);
	*derivitive = 2 * x;
}

void Cos(float x, float * evaluation, float * derivitive, float p1, float p2)
{
	assert(evaluation != nullptr);
	assert(derivitive != nullptr);

	*evaluation = mix(p1, p2, x) * abs(cos(x));
	*derivitive = -sin(x);
}


bool Cylinder::PreGLInitialize()
{
	vec3 z_axis(0.0f, 0.0f, 1.0f);

	// Cylinders are built with radius 1 and 1 unit long from -0.5 to 0.5 along the z axis.
	vec4 p(this->br, 0.0f, 0.0f, 1.0f);

	float t = 1.0f;
	float delta_t = 1.0f / float(this->stacks);

	// How far to advance between rings. 
	vec3 delta_z(0.0f, 0.0f, delta_t);

	// Rotation angle between slices.
	float theta = this->span / float(this->slices);

	float evaluation;
	float derivative;

	vec3 e(1.005f, 1.005f, 1.0f);
	float f = 1.0f + 1.0f / 7.0f;
	vec3 scale_factor_for_normals(f, f, 1.0f);

	int real_number_of_slices = this->slices + (this->is_partial_span ? 1 : 0);
	this->data.vertices.reserve(real_number_of_slices * (this->stacks + 1));
	this->data.textures.reserve(real_number_of_slices * (this->stacks + 1));
	this->data.normals.reserve(real_number_of_slices * (this->stacks + 1));
	this->data.normal_visualization_coordinates.reserve(real_number_of_slices * (this->stacks + 1) * 2);
	mat4 m = translate(mat4(), vec3(0.0f, 0.0f, -0.5f));

	for (int j = 0; j < this->stacks + 1; j++)
	{
		// Mix from back radius to front radius to derive
		// the x coordinate of the point used to project the
		// surface.
		Linear(t, &evaluation, &derivative, this->fr, this->br);

		// Given the slope from back to front, compute normal angle.
		float rho = atan(derivative);
		//cout << rho / pi<float>() * 180.0f << endl;
		vec4 rotated_y_axis = rotate(mat4(), -rho, vec3(0.0f, 1.0f, 0.0f)) * vec4(1.0f, 0.0f, 0.0f, 1.0f);
		//cout << rotated_y_axis.x << " " << rotated_y_axis.y << " " << rotated_y_axis.z << endl;

		p.x = evaluation;
		t -= delta_t;

		// Reset m2 to nothing but back to front translation.
		mat4 m2 = m;
		mat4 m3;

		// Notice j will vary up to and including this->stacks due to
		// the this->stacks + 1 up above. This allows the s parameter 
		// to range fully from 0 to 1.
		vec2 tc(j / float(this->stacks) , 0.0f);
		
		for (int i = 0; i < real_number_of_slices; i++)
		{
			// Calculate which coordinate on current stack as the t value of tc.
			// That is, vary this according to slices. If the cylinder is closed,
			// real_number_of_slices will be one greater than the number of slices
			// so the last value of the division below will be 1.0.
			//
			// The s value is computed above and varies only with each stack.
			tc = vec2(tc.s , i / float(real_number_of_slices - 1));
			this->data.textures.push_back(tc);

			vec3 n = vec3(m3 * rotated_y_axis);
			this->data.vertices.push_back(vec3(m2 * p));
			this->data.normals.push_back(n);
			this->data.normal_visualization_coordinates.push_back(*(this->data.vertices.end() - 1));
			this->data.normal_visualization_coordinates.push_back(*(this->data.vertices.end() - 1) + *(this->data.normals.end() - 1) / this->NORMAL_LENGTH_DIVISOR);
			this->data.colors.push_back(this->RandomColor((i > 0 ? *(this->data.colors.end() - 1) : vec4(0.5f, 0.5f, 0.5f, 1.0f)), -0.2f, 0.2f));
			m2 = rotate(m2, -theta, z_axis);
			m3 = rotate(m3, -theta, z_axis);
		}

		m = translate(m, delta_z);
	}


	// There will be two triangles per slice.
	// First is: current outer, current inner, next outer
	// Second is: next outer, current inner, next inner
	for (int j = 0; j < this->stacks; j++)
	{
		for (int i = 0; i < this->slices; i++)
		{
			#ifdef TRIANGLES
			// The winding is supposed to be clockwise. But this is definitely
			// counter clockwise. 
			// First or Top triangle
			this->data.indices.push_back(j * real_number_of_slices + i);
			this->data.indices.push_back(j * real_number_of_slices + (i + 1) % real_number_of_slices);
			this->data.indices.push_back((j + 1) * real_number_of_slices + i);
			// Second or Bottom triangle
			this->data.indices.push_back(j * real_number_of_slices + (i + 1) % real_number_of_slices);
			this->data.indices.push_back((j + 1) * real_number_of_slices + (i + 1) % real_number_of_slices);
			this->data.indices.push_back((j + 1) * real_number_of_slices + i);
			#else
			this->indices.push_back(j * real_number_of_slices + i);
			this->indices.push_back((j + 1) * real_number_of_slices + i);
			this->indices.push_back((j + 1) * real_number_of_slices + (i + 1) % real_number_of_slices);
			this->indices.push_back(j * real_number_of_slices + (i + 1) % real_number_of_slices);
			#endif
		}
	}
	this->data.vbackup = this->data.vertices;
	return true;
}

void Cylinder::RecomputeNormals()
{
	int real_number_of_slices = this->slices + (this->is_partial_span ? 1 : 0);
	vector<vec3> & v = data.vertices;
	vector<vec3> & n = data.normals;
	vector<vec3> & p = data.normal_visualization_coordinates;
	vec3 a;
	vec3 b;
	vec3 sum;

	int i = 0; // This is the true index of vertices.
	for (int stack_counter = 0; stack_counter < this->stacks; stack_counter++)
	{
		for (int slice_counter = 0; slice_counter < real_number_of_slices; slice_counter++, i++)
		{
			sum = vec3(0.0f);

			if (slice_counter == 0 && is_partial_span)
			{
				// This is the first vertex on this partial stack. It is treated differently
				// as there are no triangles to the left.
			}
			else if (slice_counter == slices && is_partial_span)
			{
				// This is the last vertex on this partial stack. It is treated differently 
				// as there are no triangles to the right.
			}
			else
			{
				// Except for the above, all vertices can be treated identically with the provision of course of care about wrap-around.
				//a = normalize(v[(i + 1)] - v[i]);
				//b = normalize(v[(i + 1) % real_number_of_slices] - v[i]);
			}
		}
	}
}

void Cylinder::NonGLTakeDown()
{

}

#endif