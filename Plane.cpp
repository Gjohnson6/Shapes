#include <iostream>
#include <stdexcept>
#include "Plane.h"


using namespace std;
using namespace glm;

// Width and height are one more than divisionsX and divisionsY.
// Example:
//		divisionsX = divisionsY = 2 yields a two by two grid 
//		with three verticies across and down (for a 9 by 9).
Plane::Plane(int divisionsX, int divisionsY)
{
	if (divisionsX < 1 || divisionsY < 1)
		throw std::invalid_argument("bad divisionX or divisionY value");

	this->divisionsX = divisionsX;
	this->divisionsY = divisionsY;
}

void Cube::AddTextureCoordinates()
{
	this->data.textures.push_back(vec2(0.0f , 1.0f));
	this->data.textures.push_back(vec2(1.0f , 1.0f));
	this->data.textures.push_back(vec2(1.0f , 0.0f));
	this->data.textures.push_back(vec2(0.0f , 0.0f));
}

// PreGLInitialize is used to establish an initial configuration of vertex
// attributes include position, normal, etc.
bool Cube::PreGLInitialize()
{
	// Front
	this->data.vertices.push_back(vec3(-1.0f,  1.0f,  1.0f));
	this->data.vertices.push_back(vec3( 1.0f,  1.0f,  1.0f));
	this->data.vertices.push_back(vec3( 1.0f, -1.0f,  1.0f));
	this->data.vertices.push_back(vec3(-1.0f, -1.0f,  1.0f));
	for (int i = 0; i < 4; i++)
	{
		this->data.normals.push_back(vec3(0.0f , 0.0f , 1.0f));
		this->data.normal_visualization_coordinates.push_back(this->data.vertices[0 + i]);
		this->data.normal_visualization_coordinates.push_back(this->data.vertices[0 + i] + this->data.normals[0 + i] / NORMAL_LENGTH_DIVISOR);
	}
	AddTextureCoordinates();
	// right (facing positive x axis)
	this->data.vertices.push_back(vec3(1.0f,  1.0f,  1.0f));
	this->data.vertices.push_back(vec3(1.0f,  1.0f, -1.0f));
	this->data.vertices.push_back(vec3(1.0f, -1.0f, -1.0f));
	this->data.vertices.push_back(vec3(1.0f, -1.0f,  1.0f));
	for (int i = 0; i < 4; i++)
	{
		this->data.normals.push_back(vec3(1.0f , 0.0f , 0.0f));
		this->data.normal_visualization_coordinates.push_back(this->data.vertices[4 + i]);
		this->data.normal_visualization_coordinates.push_back(this->data.vertices[4 + i] + this->data.normals[4 + i] / NORMAL_LENGTH_DIVISOR);
	}
	AddTextureCoordinates();
	// back (facing negative z axis)
	this->data.vertices.push_back(vec3( 1.0f, 1.0f, -1.0f));
	this->data.vertices.push_back(vec3(-1.0f, 1.0f, -1.0f));
	this->data.vertices.push_back(vec3(-1.0f, -1.0f, -1.0f));
	this->data.vertices.push_back(vec3( 1.0f, -1.0f, -1.0f));
	for (int i = 0; i < 4; i++)
	{
		this->data.normals.push_back(vec3(0.0f , 0.0f , -1.0f));
		this->data.normal_visualization_coordinates.push_back(this->data.vertices[8 + i]);
		this->data.normal_visualization_coordinates.push_back(this->data.vertices[8 + i] + this->data.normals[8 + i] / NORMAL_LENGTH_DIVISOR);
	}
	AddTextureCoordinates();
	// left (facing negative x axis
	this->data.vertices.push_back(vec3(-1.0f, 1.0f, -1.0f));
	this->data.vertices.push_back(vec3(-1.0f, 1.0f,  1.0f));
	this->data.vertices.push_back(vec3(-1.0f, -1.0f, 1.0f));
	this->data.vertices.push_back(vec3(-1.0f, -1.0f,-1.0f));
	for (int i = 0; i < 4; i++)
	{
		this->data.normals.push_back(vec3(-1.0f , 0.0f , 0.0f));
		this->data.normal_visualization_coordinates.push_back(this->data.vertices[12 + i]);
		this->data.normal_visualization_coordinates.push_back(this->data.vertices[12 + i] + this->data.normals[12 + i] / NORMAL_LENGTH_DIVISOR);
	}
	AddTextureCoordinates();
	// top (facing positive y axis)
	this->data.vertices.push_back(vec3(-1.0f, 1.0f, -1.0f));
	this->data.vertices.push_back(vec3( 1.0f, 1.0f, -1.0f));
	this->data.vertices.push_back(vec3( 1.0f, 1.0f,  1.0f));
	this->data.vertices.push_back(vec3(-1.0f, 1.0f,  1.0f));
	for (int i = 0; i < 4; i++)
	{
		this->data.normals.push_back(vec3(0.0f , 1.0f , 0.0f));
		this->data.normal_visualization_coordinates.push_back(this->data.vertices[16 + i]);
		this->data.normal_visualization_coordinates.push_back(this->data.vertices[16 + i] + this->data.normals[16 + i] / NORMAL_LENGTH_DIVISOR);
	}
	AddTextureCoordinates();
	// bottom (facing negative y axis)
	this->data.vertices.push_back(vec3(-1.0f, -1.0f, 1.0f));
	this->data.vertices.push_back(vec3(1.0f, -1.0f, 1.0f));
	this->data.vertices.push_back(vec3(1.0f, -1.0f, -1.0f));
	this->data.vertices.push_back(vec3(-1.0f, -1.0f, -1.0f));
	for (int i = 0; i < 4; i++)
	{
		this->data.normals.push_back(vec3(0.0f , -1.0f , 0.0f));
		this->data.normal_visualization_coordinates.push_back(this->data.vertices[20 + i]);
		this->data.normal_visualization_coordinates.push_back(this->data.vertices[20 + i] + this->data.normals[20 + i] / NORMAL_LENGTH_DIVISOR);
	}
	AddTextureCoordinates();
	
	// Faces complete.

	for (unsigned int i = 0; i < this->data.vertices.size(); i++)
		this->data.indices.push_back(i);

	this->data.vbackup = this->data.vertices;
	return true;
}

// Position vertices and push them onto ->vertices.
// Define colors if we want and push them onto ->colors.
// Define normals - this is easy - they are all the same. Push these on to normals.
// Define all the triangles by specifying indices making up top triangles and bottom triangles.
bool Plane::PreGLInitialize()
{
	float deltaX =  2.0f / float(this->divisionsX);
	float deltaY = -2.0f / float(this->divisionsY);
	vec3 v(-1.0f , 1.0f , 0.0f);

	for (int y = 0; y < this->divisionsY + 1; y++)
	{
		for (int x = 0; x < this->divisionsX + 1; x++)
		{
			// Sometimes it is useful to push the first coordinate out of its regular position
			// so that orientation of the plane can be discerned visually. When the following are
			// uncommented, the first vertex is handled differently to permit this.
			//if (y == 0 && x == 0)
			//	this->data.vertices.push_back(vec3(-2.0f, v.y, v.z));
			//else
			this->data.vertices.push_back(v);
			this->data.textures.push_back(vec2(v.x / 2.0f + 0.5f, v.y / 2.0f + 0.5f));
			this->data.normals.push_back(vec3(0.0f,0.0f,1.0f));
			this->data.colors.push_back(this->RandomColor((this->data.colors.size() > 0 ? *(this->data.colors.end() - 1) : vec4(0.5f , 0.5f , 0.5f , 1.0f)) , -0.2f , 0.2f));
			this->data.normal_visualization_coordinates.push_back(*(this->data.vertices.end() - 1));
			this->data.normal_visualization_coordinates.push_back(*(this->data.vertices.end() - 1) + vec3(0.0f, 0.0f, 1.0f) / NORMAL_LENGTH_DIVISOR);
			v.x = v.x + deltaX;
		}
		v.x = -1.0f;
		v.y = v.y + deltaY;
	}

	int i = 0;
	int w = divisionsX + 1;

	for (int y = 0; y < this->divisionsY; y++)
	{
		for (int x = 0; x < this->divisionsX; x++)
		{
			// "Top" Triangle - This associates this vertex with the one to its right with the one
			// beneath the current vertex. The order defines a clockwise winding.
			this->data.indices.push_back(i);
			this->data.indices.push_back(i + 1);
			this->data.indices.push_back(i + w);
			// Bottom triangle - This associates the vertex to our right with the one below it and
			// the one below the current vertex. This order defines a clockwise winding.
			this->data.indices.push_back(i + 1);
			this->data.indices.push_back(i + w + 1);
			this->data.indices.push_back(i + w);
			i++;
		}
		i++;
	}
	this->data.vbackup = this->data.vertices;
	return true;
}

void Plane::NonGLTakeDown()
{
}

void Cube::NonGLTakeDown()
{
}

#define	RIGHT		(1)
#define	DOWN		(this->divisionsX + 1)
#define DOWN_LEFT	(this->divisionsX + 0)
#define	LEFT		(-1)
#define UP			-DOWN
#define UP_RIGHT	-DOWN_LEFT

void Plane::SE(vec3 * v, int row , int col , glm::vec3 & sum , float & divisor)
{
	vec3 a , b, s;

	if (row == this->divisionsY || col == this->divisionsX)
		return;

	a = normalize(*(v + RIGHT) - *v);
	b = normalize(*(v + DOWN) - *v);

	s = normalize(cross(a , b));

	if (s == s)
	{
		sum += s;
		divisor++;
	}
}

void Plane::SSW(vec3 * v , int row , int col , glm::vec3 & sum , float & divisor)
{
	vec3 a , b, s;

	if (row == this->divisionsY || col == 0)
		return;

	a = normalize(*(v + DOWN) - *v);
	b = normalize(*(v + DOWN_LEFT) - *v);

	s = normalize(cross(a , b));

	if (s == s)
	{
		sum += s;
		divisor++;
	}
}

void Plane::WSW(vec3 * v , int row , int col , glm::vec3 & sum , float & divisor)
{
	vec3 a , b, s;

	if (row == this->divisionsY || col == 0)
		return;

	a = normalize(*(v + DOWN_LEFT) - *v);
	b = normalize(*(v + LEFT) - *v);

	s = normalize(cross(a , b));

	if (s == s)
	{
		sum += s;
		divisor++;
	}
}

void Plane::NW(vec3 * v , int row , int col , glm::vec3 & sum , float & divisor)
{
	vec3 a , b, s;

	if (row == 0 || col == 0)
		return;

	a = normalize(*(v + LEFT) - *v);
	b = normalize(*(v + UP) - *v);

	s = normalize(cross(a , b));

	if (s == s)
	{
		sum += s;
		divisor++;
	}
}

void Plane::NNE(vec3 * v , int row , int col , glm::vec3 & sum , float & divisor)
{
	vec3 a , b, s;

	if (row == 0 || col == this->divisionsX)
		return;

	a = normalize(*(v + UP) - *v);
	b = normalize(*(v + UP_RIGHT) - *v);

	s = normalize(cross(a , b));

	if (s == s)
	{
		sum += s;
		divisor++;
	}
}

void Plane::ENE(vec3 * v , int row , int col , glm::vec3 & sum , float & divisor)
{
	vec3 a , b, s;

	if (row == 0 || col == this->divisionsX)
		return;

	a = normalize(*(v + UP_RIGHT) - *v);
	b = normalize(*(v + RIGHT) - *v);

	s = normalize(cross(a , b));

	if (s == s)
	{
		sum += s;
		divisor++;
	}
}

void Plane::RecomputeNormals()
{
	vec3 sum;
	vec3 a;
	vec3 b;
	int i = 0;
	int w = divisionsX + 1;
	vector<vec3> & v = data.vertices;
	vector<vec3> & n = data.normals;
	vector<vec3> & p = data.normal_visualization_coordinates;

	// int i indexes the consecutive vertex indices. The row and col values are useful
	// to maintain a logical picture of the vertex index i represents. The approach here
	// is to examine every triangle in which vertex i participates. Average together all
	// the cross products (after normalizing both the input and the output) taken in 
	// clockwise order. We later determined that it should have been counter clockwise
	// order. Rather than changing all the calls to cross(), I negated the assignments
	// to data.normals (abbreviated by n).
	for (int row = 0; row <= this->divisionsY; row++)
	{
		for (int col = 0; col < w; col++, i++)
		{
			sum = vec3(0.0f);
			float divisor = 0.0f;
			SE(&v[i] , row , col , sum , divisor);
			SSW(&v[i] , row , col , sum , divisor);
			WSW(&v[i] , row , col , sum , divisor);
			NW(&v[i] , row , col , sum , divisor);
			NNE(&v[i] , row , col , sum , divisor);
			ENE(&v[i] , row , col , sum , divisor);
			assert(divisor > 0.0f);
			n[i] = -sum / divisor;
			// As we leave processing of the current vertex, we take the opportunity to
			// update the visualization vectors. The beginning of each visualization vector
			// is the position of the vertex itself. The other end of each visualization 
			// vector is the newly calculated normal modulated by a constant divisor added
			// to the position of the vertex.
			p[i * 2] = v[i];
			p[i * 2 + 1] = v[i] + n[i] / NORMAL_LENGTH_DIVISOR;
		}
	}
}

void Cube::RecomputeNormals()
{
	const int NCORNERS = 4;
	vector<vec3> & v = data.vertices;
	vector<vec3> & n = data.normals;
	vector<vec3> & p = data.normal_visualization_coordinates;
	vec3 a , b;

	int j = 0;
	// A cube is six faces of four vertices each. face_counter iterates
	// face indexes. As each face is comprised of four vertices, face_counter
	// * 4 is the base index of the 4 vertices in the face. These are i through
	// i + 3.
	// At each vertex, the line segment ahead is a where as the line segment behind
	// is b. cross(a,b) is actually backwards so the negation of the value is used
	// as the new normal.
	// Every vertex has two associated vertices in the normal visualization array.
	// These are indexed by j, allowed to post increment its way up.
	for (size_t face_counter = 0; face_counter < n.size() / NCORNERS; face_counter++)
	{
		int i = face_counter * NCORNERS;
		a = normalize(v[i + 1] - v[i + 0]);
		b = normalize(v[i + 3] - v[i + 0]);
		n[i + 0] = -normalize(cross(a , b));
		p[j++] = v[i + 0];
		p[j++] = v[i + 0] + n[i + 0] / NORMAL_LENGTH_DIVISOR;
		a = normalize(v[i + 2] - v[i + 1]);
		b = normalize(v[i + 0] - v[i + 1]);
		n[i + 1] = -normalize(cross(a , b));
		p[j++] = v[i + 1];
		p[j++] = v[i + 1] + n[i + 1] / NORMAL_LENGTH_DIVISOR;
		a = normalize(v[i + 3] - v[i + 2]);
		b = normalize(v[i + 1] - v[i + 2]);
		n[i + 2] = -normalize(cross(a , b));
		p[j++] = v[i + 2];
		p[j++] = v[i + 2] + n[i + 2] / NORMAL_LENGTH_DIVISOR;
		a = normalize(v[i + 0] - v[i + 3]);
		b = normalize(v[i + 2] - v[i + 3]);
		n[i + 3] = -normalize(cross(a , b));
		p[j++] = v[i + 3];
		p[j++] = v[i + 3] + n[i + 3] / NORMAL_LENGTH_DIVISOR;
	}
}

void Cube::Draw(bool draw_normals)
{
	if (this->data.vertices.size() == 0)
	{
		this->PreGLInitialize();
		this->CommonGLInitialization();
	}
	if (draw_normals && this->data.normal_visualization_coordinates.size() > 0)
	{
		glBindVertexArray(this->normal_array_handle);
		glDrawArrays(GL_LINES, 0, this->data.normal_visualization_coordinates.size());
	}
	else
	{
		glBindVertexArray(this->vertex_array_handle);
		glDrawElements(GL_QUADS, this->data.indices.size(), GL_UNSIGNED_INT, nullptr);
	}
	glBindVertexArray(0);
	this->GLReturnedError("Cube::Draw() - exiting");
}

Cylinder::Cylinder(int slices , int stacks , float span , float back_radius , float front_radius) : Plane(slices, stacks)
{
	float full_span = pi<float>() * 2.0f;

	if (span == 0)
		throw std::invalid_argument("bad span value");

	if (stacks < 1)
		throw std::invalid_argument("bad stack value");

	if (back_radius == 0.0f && front_radius == 0.0f)
		throw std::invalid_argument("bad radii");

	if (abs(back_radius) <= 0.00001f)
		back_radius = 0.00001f;

	if (abs(front_radius) <= 0.00001f)
		front_radius = 0.00001f;

	this->slices = abs(slices);
	this->stacks = stacks;
	this->fr = front_radius;
	this->br = back_radius;
	this->span = min<float>(full_span , abs(span));
	this->is_partial_span = this->span != full_span;
}

bool Cylinder::PreGLInitialize()
{
	// To correct for the difficulties in sharing end and beginning columns (wrap around)
	// I am reimplementing cylinder as a plane so that left and right edges are unique vertices.

	// Use the Plane implementation for making a flat plane.
	this->Plane::PreGLInitialize();

	// Now move the vertices into the shape of a cylinder.
	float derivative;
	vec3 z_axis(0.0f , 0.0f , 1.0f);
	vec4 p(this->br , 0.0f , 0.0f , 1.0f);

	// The t coordinate marches from -0.5 to 0.5 along z.
	float t = 1.0f;
	float delta_t = 1.0f / float(this->stacks);
	vec3 delta_z(0.0f , 0.0f , delta_t);

	// How far to advance between rings. 
	// Rotation angle between slices.
	float theta = this->span / float(this->slices);
	mat4 m = translate(mat4() , vec3(0.0f , 0.0f , -0.5f));

	vec3 * v = &this->data.vertices[0];
	vec3 * vn = &this->data.normal_visualization_coordinates[0];
	
	derivative = fr - br;

	for (int ring_index = 0; ring_index < this->stacks + 1; ring_index++)
	{
		// Mix from back radius to front radius to derive
		// the x coordinate of the point used to project the
		// surface.
		p.x = mix(br , fr , t);
		// Given the slope from back to front, compute normal angle.
		float rho = atan(derivative);
		//cout << rho / pi<float>() * 180.0f << endl;
		vec4 rotated_y_axis = rotate(mat4() , -rho , vec3(0.0f , 1.0f , 0.0f)) * vec4(1.0f , 0.0f , 0.0f , 1.0f);

		t -= delta_t;

		// Reset m2 to nothing but back to front translation.
		mat4 m2 = m;
		mat4 m3;

		for (int slice_index = 0; slice_index <= this->slices; slice_index++)
		{
			vec3 n = vec3(m3 * rotated_y_axis);
			*(vn++) = *v = vec3(m2 * p);
			*(vn++) = *v + n / this->NORMAL_LENGTH_DIVISOR;
			m2 = rotate(m2 , -theta , z_axis);
			m3 = rotate(m3 , -theta , z_axis);
			v++;
		}

		m = translate(m , delta_z);
	}
	this->data.vbackup = this->data.vertices;

	// Finally, rely upon the ultra cool RecomputeNormals from Plane to do all the hard work
	// for initially setting the normals.
	this->RecomputeNormals();
	return true;
}

void Cylinder::RecomputeNormals()
{
	this->Plane::RecomputeNormals();

	vector<vec3> & n = this->data.normals;
	vector<vec3> & v = this->data.vertices;
	vector<vec3> & vn = this->data.normal_visualization_coordinates;
	int w = this->slices + 1;

	if (!this->is_partial_span)
	{
		// Fix the normals of the first and last columns;
		for (int ring_index = 0; ring_index < this->stacks + 1; ring_index++)
		{
			n[ring_index * w + this->slices] = n[ring_index * w + 0] = (n[ring_index * w + 0] + n[ring_index * w + this->slices]) / 2.0f;

			vn[(ring_index * w + this->slices) * 2 + 0] = v[ring_index * w + this->slices];
			vn[(ring_index * w + this->slices) * 2 + 1] = v[ring_index * w + this->slices] + vn[(ring_index * w + this->slices) * 2 + 0] / Shape::NORMAL_LENGTH_DIVISOR;

			vn[(ring_index * w + 0) * 2 + 0] = v[ring_index * w + 0];
			vn[(ring_index * w + 0) * 2 + 1] = v[ring_index * w + 0] + vn[(ring_index * w + 0) * 2 + 0] / Shape::NORMAL_LENGTH_DIVISOR;
		}
	}
}

void Cylinder::NonGLTakeDown()
{
}
