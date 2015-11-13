#pragma once
#include "shape.h"

class Plane : public Shape
{
public:
	Plane(int divisionsX = 2, int divisionsY = 2);

	glm::ivec2 Dimensions() { return glm::ivec2(this->divisionsX , this->divisionsY); }

protected:
	bool PreGLInitialize();
	void NonGLTakeDown();
	void RecomputeNormals();

	int divisionsX;
	int divisionsY;

	void  SE(glm::vec3 * v, int row , int col , glm::vec3 & sum , float & divisor);
	void SSW(glm::vec3 * v, int row , int col , glm::vec3 & sum , float & divisor);
	void WSW(glm::vec3 * v, int row , int col , glm::vec3 & sum , float & divisor);
	void  NW(glm::vec3 * v, int row , int col , glm::vec3 & sum , float & divisor);
	void NNE(glm::vec3 * v, int row , int col , glm::vec3 & sum , float & divisor);
	void ENE(glm::vec3 * v, int row , int col , glm::vec3 & sum , float & divisor);
};

class Cylinder : public Plane
{
public:
	Cylinder(int slices , int stacks , float span , float back_radius , float front_radius);

protected:
	bool PreGLInitialize();
	void NonGLTakeDown();
	void RecomputeNormals();

	int slices;
	int stacks;
	float span;
	float fr;
	float br;
	bool is_partial_span;
};

class Cube : public Shape
{
public:
	Cube() {}
	void Draw(bool draw_normals = false);
protected:
	bool PreGLInitialize();
	void NonGLTakeDown();
	void RecomputeNormals();
	void AddTextureCoordinates();
};